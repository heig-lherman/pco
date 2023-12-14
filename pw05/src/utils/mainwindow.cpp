/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Rien à modifier

#include "mainwindow.h"
#include "ui_mainwindow.h"

#define CONSOLE_MINIMUM_WIDTH 100

MainWindow::MainWindow(unsigned nbClients, unsigned int nbPlaces, QWidget *parent)
    : QMainWindow(parent)

{
    // Setup clients consoles
    m_nbConsoles = nbClients;
    m_clientsConsoles = std::vector<QTextEdit*>(nbClients);
    m_clientsDocks = std::vector<QDockWidget*>(m_nbConsoles);

    for(unsigned int i = 0; i < nbClients; i++) {
        m_clientsConsoles[i] = new QTextEdit(this);
        m_clientsConsoles[i]->setMinimumWidth(CONSOLE_MINIMUM_WIDTH);
        m_clientsDocks[i] = new QDockWidget(this);
        m_clientsDocks[i]->setWidget(m_clientsConsoles[i]);
        if(i < nbClients/2) {
            this->addDockWidget(Qt::LeftDockWidgetArea, m_clientsDocks[i]);
        } else {
            this->addDockWidget(Qt::RightDockWidgetArea, m_clientsDocks[i]);
        }
        m_clientsDocks[i]->setWindowTitle(QString("Client number : %1").arg(i));
    }

    // Barber console
    m_barberConsole = new QTextEdit(this);
    m_barberConsole->setMinimumWidth(CONSOLE_MINIMUM_WIDTH * 2);
    m_barberDocks = new QDockWidget(this);
    m_barberDocks->setWidget(m_barberConsole);
    this->addDockWidget(Qt::BottomDockWidgetArea, m_barberDocks);
    m_barberDocks->setWindowTitle("Barber");

    // Setup the scene
    m_display = new SalonDisplay(nbClients, nbPlaces, this);
    setCentralWidget(m_display);
}

MainWindow::~MainWindow() = default;

void MainWindow::consoleAppendTextClient(unsigned int consoleId, const QString &text)
{
    if (consoleId >= m_nbConsoles)
        return;
    m_clientsConsoles[consoleId]->append(text);
}

void MainWindow::consoleAppendTextBarber(const QString &text)
{
    m_barberConsole->append(text);
}

void MainWindow::clientAccessEntrance(unsigned id){
    m_display->clientAccessEntrance(id);
}

void MainWindow::clientSitOnChair(unsigned id, int chair_nb){
    m_display->clientSitOnChair(id, chair_nb);
}

void MainWindow::clientSitOnWorkChair(unsigned id){
    m_display->clientSitOnWorkChair(id);
}

void MainWindow::clientWaitHairCut(unsigned id){
    m_display->clientWaitHairCut(id);
}

void MainWindow::clientWaitHairToGrow(unsigned id, bool comeBack){
    m_display->clientWaitHairToGrow(id, comeBack);
}

void MainWindow::clientWaitRoomIsFull(unsigned id){

}

void MainWindow::clientWalkAround(unsigned id){
    m_display->clientWalkAround(id);
}

void MainWindow::barberGoToSleep(){
    m_display->barberGoToSleep();
}

void MainWindow::barberStopSleeping(){

}

void MainWindow::barberPicksNewClient(){

}

void MainWindow::barberGoesHairCut(){

}

void MainWindow::barberCuttingHair(){
    m_display->barberCuttingHair();
}

void MainWindow::clientWakeUpBarber(){
    m_display->clientWakeUpBarber();
}

void MainWindow::clientHairCutEnded(unsigned id){

}
