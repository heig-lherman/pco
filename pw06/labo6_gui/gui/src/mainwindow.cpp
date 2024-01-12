
#include "mainwindow.h"
#include "guiinterface.h"

#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QSettings>
#include <QCoreApplication>
#include <QDockWidget>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    generalConsole = new QTextEdit(this);
    dockGeneralConsole = new QDockWidget("Console generale",this);
    dockGeneralConsole->setWidget(generalConsole);
    addDockWidget(Qt::BottomDockWidgetArea,dockGeneralConsole,Qt::Horizontal);

    setGeometry(50,50,530,580);

    setGeometry(0,0,530,580);

    // Create the computation manager (shared buffer)
    computationManager = std::make_shared<ComputationManager>();

    simView = new SimView(computationManager, this);

    setCentralWidget(simView);

    createActions();
    createMenus();
    createToolbar();
    updateMenus();


    statusLabel=new QLabel(this);
    QStatusBar *status=new QStatusBar(this);
    setStatusBar(status);
    status->addPermanentWidget(statusLabel);
    status->showMessage(tr("Pret a l'utilisation"));

    readSettings();

    this->move(100,100);
}

MainWindow::~MainWindow()
{
}


#define APPNAME "PtrWindow"
#define ORG     "REDS"

void MainWindow::readSettings()
{
    QSettings settings(ORG,APPNAME);
}

void MainWindow::writeSettings() const
{
    QSettings settings(ORG,APPNAME);

}


void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::zoomFit()
{
    simView->zoomFit();
}

void MainWindow::zoomOut()
{
    simView->zoomOut();
}

void MainWindow::zoomIn()
{
    simView->zoomIn();
}

void MainWindow::stopTasks()
{
    static bool called = false;

    if (!called) {
        called = true;
        try {
            std::thread([&](){
                GuiInterface::instance->logMessage(-1, "Stop pushed");
                // Stop the manager
                try {
                    computationManager->stop();
                } catch (ComputationManager::StopException& e) {
                    GuiInterface::instance->logMessage(-1, "Monitor threw exception (already stopped ?)");
                }

                // Wait for the computation environment to shut down
                GuiInterface::instance->logMessage(-1, "Waiting for the compute engine threads to join");
                computeEnv->joinComputeEnvironment();
                GuiInterface::instance->logMessage(-1, "Compute engines threads are joined");
            }).detach();
        } catch (...) {

        }
    } else {
        GuiInterface::instance->logMessage(-1, "Stop was already called");
    }
}


void MainWindow::logMessage(int /*threadId*/, QString message)
{
    generalConsole->append(message);
}

#include <QThread>
#include <unistd.h>

class UserThread : public QThread
{
public:
    std::shared_ptr<ComputationManager> cmp;

    UserThread(std::shared_ptr<ComputationManager> cmp) : cmp(cmp) {}

protected:

    virtual void run() {

        try {
            while (true) {
                auto res = cmp->getNextResult();
                GuiInterface::instance->logMessage(-1, QString("Got result with id : %1").arg(res.getId()));
                GuiInterface::instance->addResult(GuiInterface::instance->getCurrentTime(), QString("%1").arg(res.getId()));
            }
        } catch (ComputationManager::StopException& e) {
            GuiInterface::instance->logMessage(-1, QString("GUI stops waiting for results"));
        }
    }
};

std::string nameFromType(ComputationType ct) {
    // This is inefficient but called only once
    switch (ct) {
    case ComputationType::A : return std::string("A");
    case ComputationType::B : return std::string("B");
    case ComputationType::C : return std::string("C");
    default: return std::string("?");
    }
}

void MainWindow::launch(Computation c) {
    auto t = GuiInterface::instance->getCurrentTime();
    try {
        GuiInterface::instance->addComputeRequest(t, QString(nameFromType(c.computationType).c_str()));
        std::thread([=](){
            try {
                auto id = computationManager->requestComputation(c);
                GuiInterface::instance->addRequestStart(id, t);
            } catch (ComputationManager::StopException& e) {}
        }).detach();
    } catch (ComputationManager::StopException& e) {}
}

void MainWindow::start1()
{
    Computation sum(ComputationType::A);
    sum.data->resize(10);
    std::iota(sum.data->begin(), sum.data->end(), 0);

    launch(sum);
}

void MainWindow::start2()
{
    Computation mult(ComputationType::B);
    mult.data->resize(3);
    std::fill(mult.data->begin(), mult.data->end(), 3);

    launch(mult);
}

void MainWindow::start3()
{
    Computation div(ComputationType::C);
    div.data->resize(2);
    div.data->at(0) = 1.0;
    div.data->at(1) = 3.0;

    launch(div);
}

void MainWindow::startTasks()
{
    // Run the compute environment
    //computeEnvThread = std::make_shared<PcoThread>(runEnvironment, computationManager);
    computeEnv = std::make_shared<ComputeEnvironmentGui>(computationManager);
    computeEnv->populateComputeEnvironment();
    computeEnv->startComputeEnvironment();

    // Run the user thread
    guiThread = std::make_shared<UserThread>(computationManager);
    guiThread->start();

    stopTasksAct->setEnabled(true);
    startTasksAct->setEnabled(false);
    startTasksAct->setVisible(false);
}

void MainWindow::createToolbar()
{
    toolBar=this->addToolBar("Tools");
    toolBar->addAction(exitAct);
    toolBar->addAction(zoomInAct);
    toolBar->addAction(zoomOutAct);
    toolBar->addAction(zoomFitAct);
    toolBar->addAction(startTasksAct);
    toolBar->addAction(stopTasksAct);
    toolBar->addAction(start1Act);
    toolBar->addAction(start2Act);
    toolBar->addAction(start3Act);
}


void MainWindow::createActions()
{

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    exitAct->setIcon(QIcon("../images/simulate_exit.png"));
    CONNECT(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    startTasksAct = new QAction(tr("&Start the tasks"), this);
    startTasksAct->setShortcut(tr("Ctrl+S"));
    startTasksAct->setStatusTip(tr("Start the RT tasks"));
    startTasksAct->setIcon(QIcon("../images/simulate_start.png"));
    CONNECT(startTasksAct, SIGNAL(triggered()), this, SLOT(startTasks()));

    stopTasksAct = new QAction(tr("S&top the tasks"), this);
    stopTasksAct->setShortcut(tr("Ctrl+E"));
    stopTasksAct->setStatusTip(tr("Stop the executing tasks"));
    stopTasksAct->setIcon(QIcon("../images/simulate_end.png"));
    CONNECT(stopTasksAct, SIGNAL(triggered()), this, SLOT(stopTasks()));
    stopTasksAct->setEnabled(false);


    zoomInAct = new QAction(QIcon("../images/zoomin.png"),tr("Zoom &in"), this);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
//    zoomInAct->setShortcut(tr("Ctrl+K"));
    zoomInAct->setStatusTip(tr("Zoom in"));
    CONNECT(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));


    zoomOutAct = new QAction(QIcon("../images/zoomout.png"),tr("Zoom &out"), this);
    zoomOutAct->setShortcut(tr("Ctrl+L"));
    zoomOutAct->setStatusTip(tr("Zoom out"));
    CONNECT(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));


    zoomFitAct = new QAction(tr("Zoom &fit"), this);
    //     zoomFitAct->setShortcut(tr("Ctrl+L"));
    zoomFitAct->setStatusTip(tr("Zoom fit"));
    CONNECT(zoomFitAct, SIGNAL(triggered()), this, SLOT(zoomFit()));


    start1Act = new QAction(tr("Start A"), this);
    start1Act->setShortcut(tr("Ctrl+1"));
    start1Act->setStatusTip(tr("Start computation A"));
    CONNECT(start1Act, SIGNAL(triggered()), this, SLOT(start1()));

    start2Act = new QAction(tr("Start B"), this);
    start2Act->setShortcut(tr("Ctrl+2"));
    start2Act->setStatusTip(tr("Start computation B"));
    CONNECT(start2Act, SIGNAL(triggered()), this, SLOT(start2()));

    start3Act = new QAction(tr("Start C"), this);
    start3Act->setShortcut(tr("Ctrl+3"));
    start3Act->setStatusTip(tr("Start computation C"));
    CONNECT(start3Act, SIGNAL(triggered()), this, SLOT(start3()));

}

void MainWindow::updateMenus()
{

}

void MainWindow::createMenus()
{
    actionMenu = menuBar()->addMenu(tr("&Actions"));
    actionMenu->addAction(stopTasksAct);
    actionMenu->addAction(exitAct);

    QMenu *view=menuBar()->addMenu(tr("&Vue"));
    view->addAction(zoomInAct);
    view->addAction(zoomOutAct);
    view->addAction(zoomFitAct);
}

