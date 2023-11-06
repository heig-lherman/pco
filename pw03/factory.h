#ifndef FACTORY_H
#define FACTORY_H
#include <vector>
#include "windowinterface.h"
#include "seller.h"

class Wholesale;

/**
 * @brief La classe permet l'implémentation d'une usine et de ces fonctions
 *        de ventes et d'achats.
 */
class Factory : public Seller
{
public:
    /**
     * @brief Constructeur de la classe Factory
     * @param Fonds initiale
     * @param La ressources qui sera construite par l'usine
     * @param Vecteurs de ressources nécessaires pour construire l'objet
     */
    Factory(int uniqueId, int fund, ItemType builtItem, std::vector<ItemType> resourcesNeeded);

    /**
     * @brief Est la routine d'appel aux fonctions privée pour que l'usine fonctionne continuellement.
     * fonction threadée
     */
    void run();

    std::map<ItemType, int> getItemsForSale() override;
    int trade(ItemType it, int number) override;

    /**
     * @brief Permet d'accèder au coût du matériel produit par l'usine
     * @return Le côût du metérial produit
     */
    int getMaterialCost();

    /**
     * @brief Retourne l'identifiant de l'objet produit par l'usine
     * @return Identifiant de l'usine.
     */
    ItemType getItemBuilt();

    /**
     * @brief Cette fonction permet d'affecter à une usine pluseurs grossistes pour pouvoir échanger avec eux.
     * @param Vecteur de wholesaler
     */
    void setWholesalers(std::vector<Wholesale*> wholesalers);

    int getAmountPaidToWorkers();

    static void setInterface(WindowInterface* windowInterface);

private:
    // Liste de grossiste auxquels l'usine peut acheter des ressources
    std::vector<Wholesale*> wholesalers;
    // Liste de ressources voulus pour la production d'un objet
    const std::vector<ItemType> resourcesNeeded;
    // Identifiant de l'objet produit par l'usine, selon l'enum ItemType
    const ItemType itemBuilt;
    // Compte le nombre d'employé payé
    int nbBuild;

    static WindowInterface* interface;

    /**
     * @brief Fonction privée permettant de vérifier si l'usine à toute les ressources
     *        nécessaire à la construction d'un objet.
     * @return true si elle a assez de ressources, false sinon.
     */
    bool verifyResources();

    /**
     * @brief Achat de ressources chez les grossistes (wholesalers)
     */
    void orderResources();

    /**
     * @brief Construction d'un objet par l'usine.
     */
    void buildItem();
};


// Si dessous sont les trois types d'usines. Elle vont créer des usines en leur
// spécifiant les besoins en matérial et leur identifiant.

class PlasticFactory : public Factory{
private:
public:
    PlasticFactory(int uniqueId, int fund);
};

class ChipFactory : public Factory {
private:
public:
    ChipFactory(int uniqueId, int fund);
};

class RobotFactory : public Factory {
private:
public:
    RobotFactory(int uniqueId, int fund);
};

#endif // FACTORY_H
