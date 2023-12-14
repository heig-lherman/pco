/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 * Authors: Loïc Herman
 */
// Modifications à faire dans le fichier

#include "barber.h"
#include <unistd.h>

#include <iostream>

Barber::Barber(GraphicSalonInterface* interface, std::shared_ptr<SalonBarberInterface> salon)
    : _interface(interface), _salon(salon)
{
    _interface->consoleAppendTextBarber("Salut, prêt à travailler !");
}

void Barber::run()
{
    // Loop while salon is in service, but make sure to finish all clients in the salon before finishing
    while (_salon->isInService() || _salon->getNbClient() > 0)
    {
        _interface->consoleAppendTextBarber("Je suis prêt à accueillir un client");
        if (_salon->getNbClient() == 0)
        {
            _interface->consoleAppendTextBarber("Pas de client, je vais dormir");
            _salon->goToSleep();

            // Reiterate, will check salon is still in service and if there actually is a client
            continue;
        }

        _interface->consoleAppendTextBarber("Suivant!");
        _salon->pickNextClient();

        _interface->consoleAppendTextBarber("Je vais attendre que le client s'installe");
        _salon->waitClientAtChair();

        _interface->consoleAppendTextBarber("Je vais embellir le client");
        _salon->beautifyClient();
    }

    _interface->consoleAppendTextBarber("La journée est terminée, à demain !");
}
