/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Rien à modifier

#ifndef BARBER_H
#define BARBER_H

#include "utils/launchable.h"
#include "utils/graphicsaloninterface.h"
#include "salon.h"

class Barber : public Launchable
{
public:
    Barber(GraphicSalonInterface *interface, std::shared_ptr<SalonBarberInterface> salon);

    /*!
     * \brief Fonction lancée par le thread, représente le comportement du télécabine
     */
    void run() override;

    /*!
     * \brief Message affiché au lancement du thread
     */
    void printStartMessage() override {qDebug() << "[START] Thread du barbier lancé";}

    /*!
     * \brief Message affiché après la fin du thread
     */
    void printCompletionMessage() override {qDebug() << "[STOP] Thread du barbier a terminé";}

private:
    GraphicSalonInterface *_interface;
    std::shared_ptr<SalonBarberInterface> _salon;

};

#endif // BARBER_H
