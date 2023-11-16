#include <QApplication>
#include <QSettings>
#include <QDebug>

#include <iostream>
using namespace std;

//Header for init_maquette()
#ifdef MAQUETTE
#include "ctrain_handler.h"
#endif

//Header for CommandeTrain
#include "commandetrain.h"

/**
 * Programme principal
 */
int main(int argc, char *argv[])
{

    QApplication app(argc,argv);

    //Init the marklin maquette
#ifdef MAQUETTE
    init_maquette();
#endif

    //Init the simulator GUI
    CommandeTrain::getInstance()->init_maquette();
    return app.exec();
}
