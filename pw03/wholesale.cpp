#include "wholesale.h"
#include "factory.h"
#include "costs.h"
#include <iostream>
#include <pcosynchro/pcothread.h>

WindowInterface *Wholesale::interface = nullptr;

Wholesale::Wholesale(int uniqueId, int fund)
        : Seller(fund, uniqueId) {
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Wholesaler Created");

}

void Wholesale::setSellers(std::vector<Seller *> sellers) {
    this->sellers = sellers;

    for (Seller *seller: sellers) {
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}

void Wholesale::buyResources() {
    auto s = Seller::chooseRandomSeller(sellers);
    auto m = s->getItemsForSale();
    auto i = Seller::chooseRandomItem(m);

    if (i == ItemType::Nothing) {
        /* Nothing to buy... */
        return;
    }

    int qty = rand() % 5 + 1;
    int price = qty * getCostPerUnit(i);

    interface->consoleAppendText(
            uniqueId,
            QString("I would like to buy %1 of ").arg(qty) %
            getItemName(i) %
            QString(" which would cost me %1").arg(price)
    );

    if (money < price) {
        /* Not enough money */
        interface->consoleAppendText(uniqueId, QString("Not enough money"));
        return;
    }

    if ((price = s->trade(i, qty)) != 0) {
        mutex.lock();
        money -= price;
        stocks[i] += qty;
        interface->consoleAppendText(
                uniqueId,
                QString("[BUY] Bought %1 of ").arg(qty)
                % getItemName(i)
                % QString(" for %1").arg(price)
        );
        mutex.unlock();
    }
}

void Wholesale::run() {

    if (sellers.empty()) {
        std::cerr
                << "You have to give factories and mines to a wholeseler before launching is routine"
                << std::endl;
        return;
    }

    interface->consoleAppendText(uniqueId, "[START] Wholesaler routine");
    while (!PcoThread::thisThread()->stopRequested()) {
        buyResources();
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
        //Temps de pause pour espacer les demandes de ressources
        PcoThread::usleep((rand() % 10 + 1) * 100000);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Wholesaler routine");


}

std::map<ItemType, int> Wholesale::getItemsForSale() {
    return stocks;
}

int Wholesale::trade(ItemType it, int qty) {
    if (qty < 0 || !stocks.count(it) || stocks[it] < qty) {
        return 0;
    }

    int cost = qty * getCostPerUnit(it);

    mutex.lock();
    stocks[it] -= qty;
    money += cost;
    mutex.unlock();

    return cost;
}

void Wholesale::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}
