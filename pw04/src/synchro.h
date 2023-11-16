/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 * Authors: Loïc Herman
 */

#ifndef SYNCHRO_H
#define SYNCHRO_H

#include <QDebug>

#include <pcosynchro/pcosemaphore.h>

#include "locomotive.h"
#include "ctrain_handler.h"
#include "synchrointerface.h"

/**
 * @brief La classe Synchro implémente l'interface SynchroInterface qui
 * propose les méthodes liées à la section partagée.
 */
class Synchro final : public SynchroInterface {
public:

    /**
     * @brief Synchro Constructeur de la classe qui représente la section partagée.
     * Initialisez vos éventuels attributs ici, sémaphores etc.
     */
    Synchro() : mutex(1), stationBlock(0), stationWait(false),
                sectionBlock(0), sectionWaiting(0), sectionOccupied(false) {
    }

    /**
     * @brief access Méthode à appeler pour accéder à la section partagée
     *
     * Elle doit arrêter la locomotive et mettre son thread en attente si nécessaire.
     *
     * @param loco La locomotive qui essaie accéder à la section partagée
     */
    void access(Locomotive &loco) override {
        mutex.acquire();
        // Require wait if the section is occupied or if the loco has low priority
        // NOTE: We have to handle a case where a preempt causes an overtake since
        // the other locos could still be running.
        while (sectionOccupied || loco.priority < 0) {
            sectionWaiting++;
            loco.arreter();
            // reset priority, will allow to pass directly after waiting
            loco.priority = 0;
            afficher_message(qPrintable(QString("The engine no. %1 waits at the signal.").arg(loco.numero())));

            mutex.release();
            // Waits for the section to be free
            sectionBlock.acquire();
            mutex.acquire();

            sectionWaiting--;
            loco.demarrer();
        }

        // We have right of way!
        loco.priority = 0;
        sectionOccupied = true;
        mutex.release();

        afficher_message(qPrintable(QString("The engine no. %1 accesses the shared section.").arg(loco.numero())));
    }

    /**
     * @brief leave Méthode à appeler pour indiquer que la locomotive est sortie de la section partagée
     *
     * Réveille les threads des locomotives potentiellement en attente.
     *
     * @param loco La locomotive qui quitte la section partagée
     */
    void leave(Locomotive &loco) override {
        afficher_message(qPrintable(QString("The engine no. %1 leaves the shared section.").arg(loco.numero())));
        mutex.acquire();

        // Frees the section, also allowing the first waiting loco to pass.
        sectionOccupied = false;
        if (sectionWaiting > 0) {
            sectionBlock.release();
        }

        mutex.release();
    }

    /**
     * @brief stopAtStation Méthode à appeler quand la locomotive doit attendre à la gare
     *
     * Implémentez toute la logique que vous avez besoin pour que les locomotives
     * s'attendent correctement.
     *
     * @param loco La locomotive qui doit attendre à la gare
     */
    void stopAtStation(Locomotive &loco) override {
        afficher_message(qPrintable(QString("The engine no. %1 arrives at the station.").arg(loco.numero())));
        loco.arreter();

        mutex.acquire();
        if (stationWait) {
            // This locomotive will have priority in the shared section
            loco.priority = 10;

            // Waits for the exchange to be done
            // Mutex is still locked as we should never have more locomotives
            // moving or entering the station at this point.
            PcoThread::usleep(5e6);

            // Releases the other locomotive
            stationWait = false;
            stationBlock.release();
            mutex.release();
        } else {
            // This is the first locomotive to arrive at the station
            // It will thus not be prioritized in the shared section
            loco.priority = -10;
            stationWait = true;

            mutex.release();
            // just a block while the loco is stopped,
            // shouldn't need to bother about being preempted
            stationBlock.acquire();
        }

        loco.demarrer();
    }

private:
    PcoSemaphore sectionBlock, stationBlock, mutex;
    bool sectionOccupied, stationWait;
    int sectionWaiting;
};


#endif // SYNCHRO_H
