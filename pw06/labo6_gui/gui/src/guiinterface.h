#ifndef GUIINTERFACE_H
#define GUIINTERFACE_H


#include "mainwindow.h"
#include "computationmanager.h"

class GuiInterface: QObject
{
    Q_OBJECT
private:
    std::chrono::time_point<std::chrono::steady_clock> timeBase;
    GuiInterface();
    MainWindow *window;
public:
    void addThreadTrigger(int threadId,long long time);
    void addTask(int threadId,long long starttime,long long endtime);
    void addComputeRequest(long long time, QString text);
    void addRequestStart(int id, long long time);
    void addTaskStart(int threadId,long long time);
    void addResult(long long time, QString text);
    void addTaskEnd(int threadId,long long time);
    void addTaskExecute(int threadId,long long starttime,long long endtime);
    void logMessage(int threadId,QString string);
    void addPeriodicTask(int threadId,char *taskName,long long idate,long long period,long long deadline);
    void logSpecial(int threadId,int what,char *message,long long curTime,long long value);
    void setNbTasks(int nbTasks);
    void registerComputeEngine(int threadId, const QString name);
    void flush();
    void ending();
    std::chrono::time_point<std::chrono::steady_clock> getTimeBase() {return timeBase;}
    long long getCurrentTime() {
        auto now = std::chrono::steady_clock::now();
        auto diff = now - timeBase;
        return (long long)diff.count();
    }

    static void initialize(int argc,char *argv[]);
    static GuiInterface *instance;
    static GuiInterface* getInstance();

signals:
    void sig_addThreadTrigger(int threadId,long long time);
    void sig_addTask(int threadId,long long starttime,long long endtime);
    void sid_addComputeRequest(long long time, QString text);
    void sig_addRequestStart(int id, long long time);
    void sig_addResult(long long time, QString text);
    void sig_addTaskStart(int threadId,long long time);
    void sig_addTaskEnd(int threadId,long long time);
    void sig_addTaskExecute(int threadId,long long starttime,long long endtime);
    void sig_logSpecial(int threadId,int what,char *message,long long curTime,long long value);
    void sig_logMessage(int threadId,QString message);
    void sig_addPeriodicTask(int threadId,char *taskName,long long idate,long long period,long long deadline);
    void sig_registerComputeEngine(int threadId, const QString name);
    void sig_setNbTasks(int nbTasks);
};

#endif // GUIINTERFACE_H
