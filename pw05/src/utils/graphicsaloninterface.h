/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Rien à modifier

#ifndef GRAPHICSALONINTERFACE_H
#define GRAPHICSALONINTERFACE_H

#include <QObject>

#include "mainwindow.h"

class GraphicSalonInterface : public QObject
{
    Q_OBJECT

public:
    GraphicSalonInterface();

    static void initialize(unsigned nbClients, unsigned nbPlaces);

    void consoleAppendTextClient(unsigned int consoleId, QString text);
    void consoleAppendTextBarber(QString text);

    void clientSitOnChair(unsigned id, int chair_nb);
    void clientSitOnWorkChair(unsigned id);
    void clientHairCutEnded(unsigned id);
    void clientWaitHairToGrow(unsigned id, bool comeBack);
    void clientWalkAround(unsigned id);
    void clientAccessEntrance(unsigned id);
    void clientWakeUpBarber();


    void clientWaitHairCut(unsigned id);
    void clientWaitRoomIsFull(unsigned id);

    void barberGoToSleep();
    void barberStopSleeping();
    void barberPicksNewClient();
    void barberGoesHairCut();
    void barberCuttingHair();

protected:
    //! Indique si la fonction d'initialisation a déjà été appelée
    static bool sm_didInitialize;

    //! Fenêtre principale de l'application
    static MainWindow *mainWindow;

signals:
    void sig_consoleAppendTextClient(unsigned int consoleId,QString text);
    void sig_consoleAppendTextBarber(QString text);

    void sig_clientAccessEntrance(unsigned id);
    void sig_clientSitOnChair(unsigned id, int chair_nb);
    void sig_clientWaitHairCut(unsigned id);
    void sig_clientWaitHairToGrow(unsigned id, bool comeBack);
    void sig_clientWaitRoomIsFull(unsigned id);
    void sig_clientSitOnWorkChair(unsigned id);
    void sig_clientWalkAround(unsigned id);
    void sig_clientHairCutEnded(unsigned id);
    void sig_clientWakeUpBarber();

    void sig_barberGoToSleep();
    void sig_barberStopSleeping();
    void sig_barberPicksNewClient();
    void sig_barberGoesHairCut();
    void sig_barberCuttingHair();
};

#endif // GRAPHICSALONINTERFACE_H
