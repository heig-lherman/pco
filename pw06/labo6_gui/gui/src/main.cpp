#include <sys/mman.h>
#include <QApplication>

#include "guiinterface.h"


/**
 * Programme principal
 */
int main(int argc, char *argv[])
{
    /* Avoids memory swapping for this program */
    mlockall(MCL_CURRENT|MCL_FUTURE);

    QApplication app(argc,argv);

    // Création de l'interface graphique.
    // C'est elle qui lance ensuite les tâches temps réel
    GuiInterface::initialize(argc,argv);

    return app.exec();
}
