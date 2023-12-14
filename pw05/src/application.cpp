/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Rien à modifier

#include <iostream>
#include <pcosynchro/pcologger.h>

#include "application.h"
#include "barber.h"
#include "client.h"
#include "pcosalon.h"

Application::Application(GraphicSalonInterface *interface, unsigned nbPlaces, unsigned nbClients)
    : _interface(interface), _nbPlaces(nbPlaces), _nbClients(nbClients)
{}

void Application::run()
{
    std::vector<std::unique_ptr<Launchable>> threads(0);

    // Création du salon
    std::shared_ptr<Salon> salon = std::make_shared<PcoSalon>(_interface, _nbPlaces);

    // Création du barbier
    std::shared_ptr<Barber> barber = std::make_shared<Barber>(_interface, salon);
    barber->startThread();

    // Création des tâches clients
    for (unsigned int i = 0; i < _nbClients; ++i) {
        threads.push_back(std::make_unique<Client>(_interface, salon));
    }

    // Mélange des tâches afin d'avoir un démarrage dans un ordre aléatoire
    // Si cette ligne pose des problèmes pour la compilation, vous pouvez la
    // commenter
    std::shuffle(
        std::begin(threads),
        std::end(threads),
        std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count())
    );

    // Démarrage des threads
    for (auto& thread : threads) {
        thread->startThread();
    }

    // Attente d'une entrée utilisateur
    QTextStream qin(stdin);
    QString Line = qin.readLine();

    // Fin de service
    salon->endService();

    // Attendre la fin des threads clients
    for (auto& thread : threads) {
        thread->join();
    }

    // Attendre la fin du thread barbier
    barber->join();

    logger() << "Le programme est terminé..." << std::endl;
}
