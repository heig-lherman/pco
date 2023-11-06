#include <QApplication>

#include "utils.h"
#include "windowinterface.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Création du vecteur de thread

    WindowInterface::initialize(NB_EXTRACTOR, NB_FACTORIES, NB_WHOLESALER);
    auto interface = new WindowInterface();

    Extractor::setInterface(interface);
    Factory::setInterface(interface);
    Wholesale::setInterface(interface);

    Utils utils = Utils(NB_EXTRACTOR, NB_FACTORIES, NB_WHOLESALER);
    interface->setUtils(&utils);

    return a.exec();
}
