/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 * Authors: Loïc Herman
 */

#include "locomotivebehavior.h"
#include "ctrain_handler.h"

void LocomotiveBehavior::run()
{
    //Initialisation de la locomotive
    loco.allumerPhares();
    loco.demarrer();
    loco.afficherMessage("Ready!");

    /* A vous de jouer ! */
    while (true) {
        // Gestion de l'entrée en gare
        // Note : on s'attend donc que la locomotive soit initialisée avant la gare
        route.awaitStation();
        loco.afficherMessage("[STATION] Entering station");
        sharedSection->stopAtStation(loco);
        loco.afficherMessage("[STATION] Exiting station");

        // Gestion de l'entrée de la section
        route.awaitAccess();
        loco.afficherMessage("[SECTION] Requesting access to shared section");
        sharedSection->access(loco);
        loco.afficherMessage("[SECTION] Access granted");
        route.turnJunctions();

        // Attente de la sortie
        route.awaitLeave();
        loco.afficherMessage("[SECTION] Exiting shared section");
        sharedSection->leave(loco);
    }
}

void LocomotiveBehavior::printStartMessage()
{
    qDebug() << "[START] Thread de la loco" << loco.numero() << "lancé";
    loco.afficherMessage("Je suis lancée !");
}

void LocomotiveBehavior::printCompletionMessage()
{
    qDebug() << "[STOP] Thread de la loco" << loco.numero() << "a terminé correctement";
    loco.afficherMessage("J'ai terminé");
}
