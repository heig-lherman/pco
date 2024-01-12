#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "connect.h"
#include <QMainWindow>
#include "simview.h"

#include <QThread>
#include <QTextEdit>
#include "computationmanager.h"
#include "pcosynchro/pcothread.h"
#include "computeenvironmentgui.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    std::shared_ptr<ComputationManager> computationManager;
    std::shared_ptr<QThread> guiThread;
    std::shared_ptr<ComputeEnvironmentGui> computeEnv;
    void launch(Computation c);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int argc;
    char **argv;

    QDockWidget *dockGeneralConsole;
    QTextEdit *generalConsole;

    void readSettings();
    void writeSettings() const;


    void createActions();
    void createMenus();
    void updateMenus();
    void createToolbar();

    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *zoomFitAct;
    QAction *stopTasksAct;
    QAction *startTasksAct;

    QAction *start1Act;
    QAction *start2Act;
    QAction *start3Act;

    QMenu *actionMenu;
    QToolBar *toolBar;

    QLabel *statusLabel;

    SimView *simView;

public slots:
    void closeEvent(QCloseEvent *event);
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void stopTasks();
    void startTasks();
    void start1();
    void start2();
    void start3();
    void logMessage(int threadId,QString message);
};

#endif // MAINWINDOW_H
