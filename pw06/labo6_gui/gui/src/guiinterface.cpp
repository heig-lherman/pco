

#include <QApplication>
#include <QSvgGenerator>

#include "guiinterface.h"

GuiInterface *GuiInterface::instance=0;

void GuiInterface::ending()
{
    (dynamic_cast<QApplication*>(qApp))->closeAllWindows();
}

GuiInterface::GuiInterface()
{
    instance=this;
    window=new MainWindow();
    window->show();
    CONNECT(this,SIGNAL(sig_addThreadTrigger(int,long long)),window->simView,SLOT(addThreadTrigger(int,long long)));
    CONNECT(this,SIGNAL(sid_addComputeRequest(long long, QString)), window->simView, SLOT(addComputeRequest(long long, QString)));
    CONNECT(this,SIGNAL(sig_addRequestStart(int, long long)), window->simView,SLOT(addRequestStart(int, long long)));
    CONNECT(this,SIGNAL(sig_addResult(long long, QString)), window->simView, SLOT(addResult(long long, QString)));
    CONNECT(this,SIGNAL(sig_addTaskStart(int,long long)),window->simView,SLOT(addTaskStart(int,long long)));
    CONNECT(this,SIGNAL(sig_addTaskEnd(int,long long)),window->simView,SLOT(addTaskEnd(int,long long)));
    CONNECT(this,SIGNAL(sig_addTask(int,long long,long long)),window->simView,SLOT(addTask(int,long long,long long)));
    CONNECT(this,SIGNAL(sig_addTaskExecute(int,long long,long long)),window->simView,SLOT(addTaskExecute(int,long long,long long)));
    CONNECT(this,SIGNAL(sig_logMessage(int,QString)),window,SLOT(logMessage(int,QString)));
    CONNECT(this,SIGNAL(sig_addPeriodicTask(int,char *,long long,long long,long long)),window->simView,SLOT(addPeriodicTask(int,char *,long long,long long,long long)));
    CONNECT(this,SIGNAL(sig_registerComputeEngine(int, const QString)),window->simView,SLOT(registerComputeEngine(int, const QString)));
    CONNECT(this,SIGNAL(sig_setNbTasks(int)),window->simView,SLOT(setNbTasks(int)));
    CONNECT(this,SIGNAL(sig_logSpecial(int,int,char*,long long,long long)),window->simView,SLOT(logSpecial(int,int,char*,long long,long long)));
    timeBase = std::chrono::steady_clock::now();
}

void GuiInterface::initialize(int argc, char *argv[])
{
    if (instance==0)
        instance=new GuiInterface();
    instance->window->argc=argc;
    instance->window->argv=argv;
	instance->window->startTasks();
}

GuiInterface* GuiInterface::getInstance()
{
    if (instance==0)
        initialize(0,0);
    return instance;
}

void GuiInterface::addThreadTrigger(int threadId,long long time)
{
    emit sig_addThreadTrigger(threadId,time);
}

void GuiInterface::addComputeRequest(long long time, QString text)
{
    emit sid_addComputeRequest(time, text);
}

void GuiInterface::addTask(int threadId, long long starttime, long long endtime)
{
    emit sig_addTask(threadId,starttime,endtime);
}

void GuiInterface::addRequestStart(int id, long long time)
{
    emit sig_addRequestStart(id, time);
}

void GuiInterface::addTaskStart(int threadId,long long time)
{
    emit sig_addTaskStart(threadId,time);
}

void GuiInterface::addResult(long long time, QString text)
{
    emit sig_addResult(time, text);
}

void GuiInterface::addTaskEnd(int threadId,long long time)
{
    emit sig_addTaskEnd(threadId,time);
}

void GuiInterface::addTaskExecute(int threadId,long long starttime,long long endtime)
{
    emit sig_addTaskExecute(threadId,starttime,endtime);
}

void GuiInterface::logSpecial(int threadId, int what, char *message, long long curTime, long long value)
{
    emit sig_logSpecial(threadId,what,message,curTime,value);
}

void GuiInterface::logMessage(int threadId,QString message)
{
    emit sig_logMessage(threadId,message);
}

void GuiInterface::addPeriodicTask(int threadId,char *taskName,long long idate,long long period,long long deadline)
{
    emit sig_addPeriodicTask(threadId,taskName,idate,period,deadline);
}

void GuiInterface::setNbTasks(int nbTasks)
{
    emit sig_setNbTasks(nbTasks);
}

void GuiInterface::registerComputeEngine(int threadId, const QString name)
{
    emit sig_registerComputeEngine(threadId, name);
}

void GuiInterface::flush()
{
    QGraphicsView *view = window->simView;
    QGraphicsScene *scene = view->scene();
    scene->setBackgroundBrush(QBrush(Qt::white, Qt::SolidPattern));
    scene->clearSelection();                                                  // Selections would also render to the file
    scene->setSceneRect(scene->itemsBoundingRect());                          // Re-shrink the scene to it's bounding contents

    {
        QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
        image.fill(Qt::transparent);                                              // Start all pixels transparent

        QPainter painter(&image);
        scene->render(&painter);
        image.save("scheduling.png");
    }

    {
        QSvgGenerator generator;
        generator.setFileName("scheduling.svg");
        generator.setSize(scene->sceneRect().size().toSize());
        generator.setViewBox(scene->sceneRect());
        generator.setTitle(tr("SVG Scheduling"));
        generator.setDescription(tr("SVG Scheduling"));
        QPainter painter(&generator);
        scene->render(&painter);
    }
}
