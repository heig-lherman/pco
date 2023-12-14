/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Rien à modifier

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QDockWidget>

#include "display.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(unsigned int nbClients, unsigned int nbPlaces, QWidget *parent = nullptr);
    ~MainWindow();

    std::vector<QDockWidget* > m_clientsDocks;
    std::vector<QTextEdit* > m_clientsConsoles;
    QDockWidget* m_barberDocks;
    QTextEdit* m_barberConsole;
    SalonDisplay *m_display;

private:
    Ui::MainWindow *ui;
    unsigned int m_nbConsoles;

public slots:
    void consoleAppendTextClient(unsigned int consoleId, const QString &text);
    void consoleAppendTextBarber(const QString &text);

    void clientAccessEntrance(unsigned id);
    void clientSitOnChair(unsigned id, int chair_nb);
    void clientWaitHairCut(unsigned id);
    void clientWaitHairToGrow(unsigned id, bool comeBack);
    void clientWaitRoomIsFull(unsigned id);
    void clientSitOnWorkChair(unsigned id);
    void clientWalkAround(unsigned id);
    void clientHairCutEnded(unsigned id);
    void clientWakeUpBarber();

    void barberGoToSleep();
    void barberStopSleeping();
    void barberPicksNewClient();
    void barberGoesHairCut();
    void barberCuttingHair();
};
#endif // MAINWINDOW_H
