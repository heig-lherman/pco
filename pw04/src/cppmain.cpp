/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 * Authors: Loïc Herman
 */

#include "ctrain_handler.h"

#include "locomotive.h"
#include "locomotivebehavior.h"
#include "synchrointerface.h"
#include "synchro.h"
#include "route.h"

// Locomotives :
// Vous pouvez changer les vitesses initiales, ou utiliser la fonction loco.fixerVitesse(vitesse);
// Laissez les numéros des locos à 0 et 1 pour ce laboratoire

// Locomotive A
static Locomotive locoA(7 /* Numéro (pour commande trains sur maquette réelle) */, 12 /* Vitesse */);
// Locomotive B
static Locomotive locoB(42 /* Numéro (pour commande trains sur maquette réelle) */, 10 /* Vitesse */);

//Arret d'urgence
void emergency_stop()
{
    locoA.arreter();
    locoB.arreter();
    locoA.fixerVitesse(0);
    locoB.fixerVitesse(0);

    afficher_message("\nSTOP!");
}


//Fonction principale
int cmain()
{
    /************
     * Maquette *
     ************/

    //Choix de la maquette (A ou B)
    selection_maquette(MAQUETTE_A);
    // selection_maquette(MAQUETTE_B);

    /**********************************
     * Initialisation des aiguillages *
     **********************************/

    // Initialisation des aiguillages
    // Positiion de base donnée comme exemple, vous pouvez la changer comme bon vous semble
    // Vous devrez utiliser cette fonction pour la section partagée pour aiguiller les locos
    // sur le bon parcours (par exemple à la sortie de la section partagée) vous pouvez l'
    // appeler depuis vos thread des locos par ex.
    diriger_aiguillage(1,  TOUT_DROIT, 0);
    diriger_aiguillage(2,  DEVIE     , 0);
    diriger_aiguillage(3,  TOUT_DROIT, 0);
    diriger_aiguillage(4,  DEVIE     , 0);
    diriger_aiguillage(5,  DEVIE     , 0);
    diriger_aiguillage(6,  DEVIE     , 0);
    diriger_aiguillage(7,  TOUT_DROIT, 0);
    diriger_aiguillage(8,  DEVIE     , 0);
    diriger_aiguillage(9,  DEVIE     , 0);
    diriger_aiguillage(10, TOUT_DROIT, 0);
    diriger_aiguillage(11, DEVIE     , 0);
    diriger_aiguillage(12, DEVIE     , 0);
    diriger_aiguillage(13, TOUT_DROIT, 0);
    diriger_aiguillage(14, DEVIE     , 0);
    diriger_aiguillage(15, DEVIE     , 0);
    diriger_aiguillage(16, TOUT_DROIT, 0);
    diriger_aiguillage(17, DEVIE     , 0);
    diriger_aiguillage(18, DEVIE     , 0);
    diriger_aiguillage(19, DEVIE     , 0);
    diriger_aiguillage(20, TOUT_DROIT, 0);
    diriger_aiguillage(21, DEVIE     , 0);
    diriger_aiguillage(22, TOUT_DROIT, 0);
    diriger_aiguillage(23, DEVIE     , 0);
    diriger_aiguillage(24, DEVIE     , 0);
    // diriger_aiguillage(/*NUMERO*/, /*TOUT_DROIT | DEVIE*/, /*0*/);

    /********************************
     * Position de départ des locos *
     ********************************/

    // Loco 0
    // Exemple de position de départ
    locoA.fixerPosition(18, 17);

    // Loco 1
    // Exemple de position de départ
    locoB.fixerPosition(22, 28);

    /***********
     * Message *
     **********/

    // Affiche un message dans la console de l'application graphique
    afficher_message("Hit play to start the simulation...");

    /************
     * Parcours *
     ***********/

    std::vector<RailwayRoute::Contact> pathA{16, 23, 24, 26, 27, 9, 8, 6, 5, 34, 33, 36, 35, 17, 18, 15};
    std::vector<RailwayRoute::Junction> junctionsA{
            {3, DEVIE},
            {20, DEVIE},
    };

    std::vector<RailwayRoute::Contact> pathB{22, 21, 20, 19, 13, 12, 11, 10, 4, 6, 5, 34, 33, 28};
    std::vector<RailwayRoute::Junction> junctionsB{
            {3, TOUT_DROIT},
            {20, TOUT_DROIT},
    };

    RailwayRoute::SharedSection shared{6, 33};

    RailwayRoute routeA(pathA, junctionsA, shared, 16);
    RailwayRoute routeB(pathB, junctionsB, shared, 19, 3);

    /*********************
     * Threads des locos *
     ********************/

    // Création de la section partagée
    std::shared_ptr<SynchroInterface> sharedSection = std::make_shared<Synchro>();

    // Création du thread pour la loco 0
    std::unique_ptr<Launchable> locoBehaveA = std::make_unique<LocomotiveBehavior>(locoA, sharedSection, routeA);
    // Création du thread pour la loco 1
    std::unique_ptr<Launchable> locoBehaveB = std::make_unique<LocomotiveBehavior>(locoB, sharedSection, routeB);

    // Lanchement des threads
    afficher_message(qPrintable(QString("Lancement thread loco A (numéro %1)").arg(locoA.numero())));
    locoBehaveA->startThread();
    afficher_message(qPrintable(QString("Lancement thread loco B (numéro %1)").arg(locoB.numero())));
    locoBehaveB->startThread();

    // Attente sur la fin des threads
    locoBehaveA->join();
    locoBehaveB->join();

    //Fin de la simulation
    mettre_maquette_hors_service();

    return EXIT_SUCCESS;
}
