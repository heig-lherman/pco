#include "factory.h"
#include "extractor.h"
#include "costs.h"
#include "wholesale.h"
#include <pcosynchro/pcothread.h>
#include <iostream>

WindowInterface *Factory::interface = nullptr;


Factory::Factory(int uniqueId, int fund, ItemType builtItem,
                 std::vector<ItemType> resourcesNeeded)
        : Seller(fund, uniqueId), resourcesNeeded(resourcesNeeded),
          itemBuilt(builtItem), nbBuild(0) {
    assert(builtItem == ItemType::Chip ||
           builtItem == ItemType::Plastic ||
           builtItem == ItemType::Robot);

    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Factory created");
}

void Factory::setWholesalers(std::vector<Wholesale *> wholesalers) {
    Factory::wholesalers = wholesalers;

    for (Seller *seller: wholesalers) {
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}

ItemType Factory::getItemBuilt() {
    return itemBuilt;
}

int Factory::getMaterialCost() {
    return getCostPerUnit(itemBuilt);
}

bool Factory::verifyResources() {
    for (auto item: resourcesNeeded) {
        if (stocks[item] == 0) {
            return false;
        }
    }

    return true;
}

void Factory::buildItem() {
    int salary = getEmployeeSalary(getEmployeeThatProduces(itemBuilt));
    if (money < salary) {
        /* Pas assez d'argent */
        /* Attend des jours meilleurs */
        PcoThread::usleep(1000U);
        return;
    }

    mutex.lock();
    /* On peut payer un employé */
    money -= salary;
    for (auto item: resourcesNeeded) {
        --stocks[item];
    }
    mutex.unlock();

    // Temps simulant l'assemblage d'un objet.
    PcoThread::usleep((rand() % 100) * 100000);

    mutex.lock();
    ++nbBuild;
    ++stocks[itemBuilt];
    mutex.unlock();

    interface->consoleAppendText(uniqueId, "Factory have build a new object");
}

void Factory::orderResources() {
    ItemType itemNeeded;
    for (auto res: resourcesNeeded) {
        itemNeeded = res;
        if (stocks[res] == 0) {
            break;
        }
    }

    int price = getCostPerUnit(itemNeeded);
    if (money < price) {
        return;
    }

    for (auto seller: wholesalers) {
        if ((price = seller->trade(itemNeeded, 1)) != 0) {
            mutex.lock();
            money -= price;
            stocks[itemNeeded] += 1;
            mutex.unlock();

            interface->consoleAppendText(
                    uniqueId,
                    QString("[BUY] Factory have bought 1 ")
                    % getItemName(itemNeeded)
                    % QString(" for %1").arg(price)
                    % QString(" from wholesaler %1").arg(seller->getUniqueId())
            );
            break;
        }
    }

    //Temps de pause pour éviter trop de demande
    PcoThread::usleep(10 * 100000);
}

void Factory::run() {
    if (wholesalers.empty()) {
        std::cerr
                << "You have to give to factories wholesalers to sales their resources"
                << std::endl;
        return;
    }
    interface->consoleAppendText(uniqueId, "[START] Factory routine");

    while (!PcoThread::thisThread()->stopRequested()) {
        if (verifyResources()) {
            buildItem();
        } else {
            orderResources();
        }
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Factory routine");
}

std::map<ItemType, int> Factory::getItemsForSale() {
    return std::map<ItemType, int>({{itemBuilt, stocks[itemBuilt]}});
}

int Factory::trade(ItemType it, int qty) {
    if (qty < 0 || it != itemBuilt || stocks[it] < qty) {
        return 0;
    }

    mutex.lock();
    int cost = qty * getCostPerUnit(it);
    stocks[it] -= qty;
    money += cost;
    mutex.unlock();

    return cost;
}

int Factory::getAmountPaidToWorkers() {
    return Factory::nbBuild *
           getEmployeeSalary(getEmployeeThatProduces(itemBuilt));
}

void Factory::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}

PlasticFactory::PlasticFactory(int uniqueId, int fund) :
        Factory::Factory(uniqueId, fund, ItemType::Plastic,
                         {ItemType::Petrol}) {}

ChipFactory::ChipFactory(int uniqueId, int fund) :
        Factory::Factory(uniqueId, fund, ItemType::Chip,
                         {ItemType::Sand, ItemType::Copper}) {}

RobotFactory::RobotFactory(int uniqueId, int fund) :
        Factory::Factory(uniqueId, fund, ItemType::Robot,
                         {ItemType::Chip, ItemType::Plastic}) {}
