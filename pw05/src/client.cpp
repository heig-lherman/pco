/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 * Authors: Loïc Herman
 */
// Modifications à faire dans le fichier

#include "client.h"
#include <unistd.h>

#include <iostream>

int Client::_nextId = 0;

Client::Client(GraphicSalonInterface* interface, std::shared_ptr<SalonClientInterface> salon)
    : _interface(interface), _salon(salon), _clientId(_nextId++)
{
    _interface->consoleAppendTextClient(_clientId, "Salut, prêt pour une coupe !");
}

void Client::run()
{
    while (_salon->isInService())
    {
        _interface->consoleAppendTextClient(_clientId, "Je souhaite accéder au salon");
        if (!_salon->accessSalon(_clientId))
        {
            _interface->consoleAppendTextClient(_clientId, "Le salon est plein, je vais me balader");
            _salon->walkAround(_clientId);

            continue;
        }

        _interface->consoleAppendTextClient(_clientId, "Je vais me faire coiffer");
        _salon->goForHairCut(_clientId);

        _interface->consoleAppendTextClient(_clientId, "Je vais attendre que mes cheveux repoussent");
        _salon->waitingForHairToGrow(_clientId);
    }

    _interface->consoleAppendTextClient(_clientId, "Le salon est fermé... Zut !");
    _salon->goHome(_clientId);
}
