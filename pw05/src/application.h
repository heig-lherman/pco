/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Rien à modifier

#ifndef APPLICATION_H
#define APPLICATION_H

#include "utils/graphicsaloninterface.h"
#include "utils/launchable.h"

class Application : public Launchable
{
public:
    Application(GraphicSalonInterface *interface, unsigned nbPlaces, unsigned nbClients);

    /*!
     * \brief Fonction principale du programme
     */
    void run() override;

    /*!
     * \brief Message affiché lors du démarrage du thread
     */
    void printStartMessage() override {qDebug() << "[START] Application lancée";}

    /*!
     * \brief Message affiché lorsque le thread a terminé
     */
    void printCompletionMessage() override {qDebug() << "[STOP] L'application est terminé";}

private:
    GraphicSalonInterface *_interface;
    unsigned _nbPlaces;
    unsigned _nbClients;
};

#endif // APPLICATION_H
