#include <pcosynchro/pcothread.h>
#include "extractor.h"
#include "costs.h"

WindowInterface* Extractor::interface = nullptr;

Extractor::Extractor(int uniqueId, int fund, ItemType resourceExtracted)
    : Seller(fund, uniqueId), resourceExtracted(resourceExtracted), nbExtracted(0)
{
    assert(resourceExtracted == ItemType::Copper ||
           resourceExtracted == ItemType::Sand ||
           resourceExtracted == ItemType::Petrol);
    interface->consoleAppendText(uniqueId, QString("Mine Created"));
    interface->updateFund(uniqueId, fund);
}

std::map<ItemType, int> Extractor::getItemsForSale() {
    return stocks;
}

int Extractor::trade(ItemType it, int qty) {
    if (qty < 0 || it != getResourceMined() || stocks[it] < qty) {
        return 0;
    }

    int cost = qty * getCostPerUnit(it);

    mutex.lock();
    stocks[it] -= qty;
    money += cost;
    mutex.unlock();

    return cost;
}

void Extractor::run() {
    interface->consoleAppendText(uniqueId, "[START] Mine routine");

    while (!PcoThread::thisThread()->stopRequested()) {
        int minerCost = getEmployeeSalary(getEmployeeThatProduces(resourceExtracted));
        if (money < minerCost) {
            /* Pas assez d'argent */
            /* Attend des jours meilleurs */
            PcoThread::usleep(1000U);
            continue;
        }

        mutex.lock();
        /* On peut payer un mineur */
        money -= minerCost;
        mutex.unlock();

        /* Temps aléatoire borné qui simule le mineur qui mine */
        PcoThread::usleep((rand() % 100 + 1) * 10000);

        mutex.lock();
        /* Statistiques */
        nbExtracted++;
        /* Incrément des stocks */
        stocks[resourceExtracted] += 1;
        mutex.unlock();

        /* Message dans l'interface graphique */
        interface->consoleAppendText(uniqueId, QString("1 ") % getItemName(resourceExtracted) %
                                     " has been mined");
        /* Update de l'interface graphique */
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
    }

    interface->consoleAppendText(uniqueId, "[STOP] Mine routine");
}

int Extractor::getMaterialCost() {
    return getCostPerUnit(resourceExtracted);
}

ItemType Extractor::getResourceMined() {
    return resourceExtracted;
}

int Extractor::getAmountPaidToMiners() {
    return nbExtracted * getEmployeeSalary(getEmployeeThatProduces(resourceExtracted));
}

void Extractor::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}

SandExtractor::SandExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Sand) {}

CopperExtractor::CopperExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Copper) {}

PetrolExtractor::PetrolExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Petrol) {}
