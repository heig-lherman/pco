#ifndef SELLER_H
#define SELLER_H

#include <QString>
#include <QStringBuilder>
#include <pcosynchro/pcomutex.h>
#include <map>
#include <vector>
#include "costs.h"

enum class ItemType { Sand, Copper, Petrol, Chip, Plastic, Robot, Nothing};

int getCostPerUnit(ItemType item);
QString getItemName(ItemType item);

enum class EmployeeType {Extractor, Electrician, Plasturgist, Engineer};

EmployeeType getEmployeeThatProduces(ItemType item);
int getEmployeeSalary(EmployeeType employee);

class Seller {
public:
    /**
     * @brief Seller
     * @param money money money !
     */
    Seller(int money, int uniqueId) : money(money), uniqueId(uniqueId) {}

    /**
     * @brief getItemsForSale
     * @return The list of items for sale
     */
    virtual std::map<ItemType, int> getItemsForSale() = 0;

    /**
     * @brief Fonction permettant d'acheter des ressources au vendeur
     * @param Le type de ressource à acheter
     * @param Nombre de ressources voulant être achetées
     * @return La facture : côut de la ressource * le nombre, 0 si indisponible
     */
    virtual int trade(ItemType what, int qty) = 0;

    /**
     * @brief chooseRandomSeller
     * @param sellers
     * @return Returns a random seller from the sellers vector
     */
    static Seller* chooseRandomSeller(std::vector<Seller*>& sellers);

    /**
     * @brief Chooses a random item type from an items for sale map
     * @param itemsForSale
     * @return Returns the item type
     */
    static ItemType chooseRandomItem(std::map<ItemType, int>& itemsForSale);

    int getFund() { return money; }

    int getUniqueId() { return uniqueId; }

protected:
    PcoMutex mutex;

    /**
     * @brief stocks : Type, Quantité
     */
    std::map<ItemType, int> stocks;
    int money;
    int uniqueId;
};

#endif // SELLER_H
