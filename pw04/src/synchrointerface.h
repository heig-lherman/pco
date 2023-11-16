/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */

// Rien à modifier ici

#ifndef SYNCHROINTERFACE_H
#define SYNCHROINTERFACE_H

#include "locomotive.h"

/**
 * @brief La classe SynchroInterface est une interface (classe abstraite pure) qui définit
 * trois méthodes: access, leave et stopAtStation afin de gérer l'accès à une section partagée d'un
 * parcours de locomotives.
 */
class SynchroInterface
{

public:

    /**
     * @brief access Méthode à appeler pour accéder à la section partagée
     *
     * Elle doit arrêter la locomotive et mettre son thread en attente si nécessaire.
     *
     * @param loco La locomotive qui essaie accéder à la section partagée
     */
    virtual void access(Locomotive& loco) = 0;

    /**
     * @brief leave Méthode à appeler pour indiquer que la locomotive est sortie de la section partagée
     *
     * Réveille les threads des locomotives potentiellement en attente.
     *
     * @param loco La locomotive qui quitte la section partagée
     */
    virtual void leave(Locomotive& loco) = 0;

    /**
     * @brief stopAtStation Méthode à appeler quand la locomotive doit attendre à la gare
     *
     * Implémentez toute la logique que vous avez besoin pour que les locomotives
     * s'attendent correctement.
     *
     * @param loco La locomotive qui doit attendre à la gare
     */
    virtual void stopAtStation(Locomotive& loco) = 0;
};

#endif // SYNCHROINTERFACE_H
