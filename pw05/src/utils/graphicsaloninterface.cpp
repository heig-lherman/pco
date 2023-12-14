/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Rien à modifier

#include <iostream>
#include <QMessageBox>

#include "graphicsaloninterface.h"
#include "mainwindow.h"

using namespace std;


bool GraphicSalonInterface::sm_didInitialize   = false;
MainWindow *GraphicSalonInterface::mainWindow  = nullptr;

GraphicSalonInterface::GraphicSalonInterface()
{
    if (!sm_didInitialize) {
        cout << "Vous devez appeler PSlideInterface::initialize()" << endl;
        exit(-1);
    }

    if (!QObject::connect(this, SIGNAL(sig_consoleAppendTextClient(unsigned int, QString)), mainWindow,
                          SLOT(consoleAppendTextClient(unsigned int, QString)), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_consoleAppendTextBarber(QString)), mainWindow,
                          SLOT(consoleAppendTextBarber(QString)), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_clientAccessEntrance(unsigned)), mainWindow,
                          SLOT(clientAccessEntrance(unsigned)), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_clientSitOnChair(unsigned, int)), mainWindow,
                          SLOT(clientSitOnChair(unsigned, int)), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_clientSitOnWorkChair(unsigned)), mainWindow,
                          SLOT(clientSitOnWorkChair(unsigned)), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_clientWaitHairCut(unsigned)), mainWindow,
                          SLOT(clientWaitHairCut(unsigned)), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_clientWaitHairToGrow(unsigned, bool)), mainWindow,
                          SLOT(clientWaitHairToGrow(unsigned, bool)), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_clientWaitRoomIsFull(unsigned)), mainWindow,
                          SLOT(clientWaitRoomIsFull(unsigned)), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_clientWalkAround(unsigned)), mainWindow,
                          SLOT(clientWalkAround(unsigned)), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_barberGoToSleep()), mainWindow,
                          SLOT(barberGoToSleep()), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_barberStopSleeping()), mainWindow,
                          SLOT(barberStopSleeping()), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_barberPicksNewClient()), mainWindow,
                          SLOT(barberPicksNewClient()), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_barberGoesHairCut()), mainWindow,
                          SLOT(barberGoesHairCut()), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_barberCuttingHair()), mainWindow,
                          SLOT(barberCuttingHair()), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_clientHairCutEnded(unsigned)), mainWindow,
                          SLOT(clientHairCutEnded(unsigned)), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this, SIGNAL(sig_clientWakeUpBarber()), mainWindow,
                          SLOT(clientWakeUpBarber()), Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }
}

void GraphicSalonInterface::initialize(unsigned nbClients, unsigned nbPlaces)
{
    if (sm_didInitialize) {
        cout << "Vous devez ne devriez appeler PSlideInterface::initialize() qu'une seule fois" << endl;
        return;
    }

    mainWindow = new MainWindow(nbClients, nbPlaces, nullptr);
    mainWindow->show();
    sm_didInitialize=true;
}

void GraphicSalonInterface::consoleAppendTextClient(unsigned int consoleId, QString text)
{
    emit sig_consoleAppendTextClient(consoleId, std::move(text));
}


void GraphicSalonInterface::consoleAppendTextBarber(QString text)
{
    emit sig_consoleAppendTextBarber(std::move(text));
}

void GraphicSalonInterface::clientAccessEntrance(unsigned int id){
    emit sig_clientAccessEntrance(id);
    getClient(id)->sem.acquire();
}

void GraphicSalonInterface::clientSitOnChair(unsigned int id, int chair_nb){
    emit sig_clientSitOnChair(id, chair_nb);
//    getClient(id)->sem.acquire();
}

void GraphicSalonInterface::clientWaitHairCut(unsigned int id){
    emit sig_clientWaitHairCut(id);
}

void GraphicSalonInterface::clientWaitHairToGrow(unsigned int id, bool comeBack){
    if(comeBack){
        emit sig_clientAccessEntrance(id);
        getClient(id)->sem.acquire();
    }
    emit sig_clientWaitHairToGrow(id, comeBack);
    getClient(id)->sem.acquire();
}

void GraphicSalonInterface::clientWaitRoomIsFull(unsigned int id){
    emit sig_clientWaitRoomIsFull(id);
}

void GraphicSalonInterface::clientSitOnWorkChair(unsigned id){
    emit sig_clientSitOnWorkChair(id);
    getClient(id)->sem.acquire();
}

void GraphicSalonInterface::clientWalkAround(unsigned int id){
    emit sig_clientWalkAround(id);
    getClient(id)->sem.acquire();
}

void GraphicSalonInterface::barberGoToSleep(){
    emit sig_barberGoToSleep();
    //getBarber()->sem.acquire();
}

void GraphicSalonInterface::barberStopSleeping(){
    emit sig_barberStopSleeping();
    getBarber()->sem.acquire();
}

void GraphicSalonInterface::barberPicksNewClient(){
    emit sig_barberPicksNewClient();
}

void GraphicSalonInterface::barberGoesHairCut(){
    emit sig_barberGoesHairCut();
}

void GraphicSalonInterface::barberCuttingHair(){
    emit sig_barberCuttingHair();
    getBarber()->sem.acquire();
}

void GraphicSalonInterface::clientHairCutEnded(unsigned int id){
    emit sig_clientHairCutEnded(id);
}

void GraphicSalonInterface::clientWakeUpBarber(){
    emit sig_clientWakeUpBarber();
    getBarber()->sem.acquire();
}
