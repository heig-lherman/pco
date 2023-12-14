/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Rien à modifier mis à part les constantes pour
// tester différents setups

#include <QApplication>
#include <pcosynchro/pcologger.h>

#include "utils/mainwindow.h"
#include "application.h"
#include "utils/graphicsaloninterface.h"

constexpr unsigned int NB_CLIENTS = 10;
constexpr unsigned int NB_SIEGES = 2;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    logger().setVerbosity(1);

    // Initialisation de la partie graphique de l'application
    GraphicSalonInterface::initialize(NB_CLIENTS, NB_SIEGES);

    // Création de l'interface pour les commandes à la partie graphique
    auto guiInterface = new GraphicSalonInterface();

    Application app(guiInterface, NB_SIEGES, NB_CLIENTS);

    app.startThread();

    return a.exec();
}
