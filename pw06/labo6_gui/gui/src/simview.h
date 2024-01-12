#ifndef SIMVIEW_H
#define SIMVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsRectItem>

#include "connect.h"
#include "computationmanager.h"

class PeriodicTask
{
public:
    PeriodicTask(int taskId,char *taskName,long long idate,long long period,long long deadline)
    {
        this->taskId=taskId;
        this->idate=idate;
        this->period=period;
        this->nextReq=-1;
        this->deadline = deadline;
        this->taskName = taskName;
    }

    int taskId;
    long long idate;
    long long period;

    long long nextReq;
    long long deadline;
    char *taskName;
};


class ComputeRequestItem : public QGraphicsRectItem
{
public:
    int id{-1};
    void setId(int i) {id = i; setBrush(QColor(0,0,255));}
};

class SimView : public QGraphicsView
{
    Q_OBJECT

private:
    std::map<int, std::string> threadNames;
    std::shared_ptr<ComputationManager> computationManager;
public:
    explicit SimView(const std::shared_ptr<ComputationManager>& computationManager, QWidget *parent =0);
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void redraw();
    void checkTask(int taskId);

    QList<QGraphicsSimpleTextItem *> textItemsList;
    QList<QGraphicsRectItem *> rectItemsList;
    QList<QGraphicsLineItem *> lineItemsList;


    QGraphicsSimpleTextItem *addQGraphicsSimpleTextItem(QString text);
    QGraphicsRectItem *addQGraphicsRectItem(qreal x, qreal y, qreal width, qreal height);
    QGraphicsLineItem *addQGraphicsLineItem( qreal x1, qreal y1, qreal x2, qreal y2);

    QList<PeriodicTask*> tasksList;
    QList<ComputeRequestItem*> computeRequestList;
    long long nextTime;

    void updatePeriodicTasks(long long time);

    int nbTasks;

public slots:
    void addThreadTrigger(int threadId,long long time);
    void addComputeRequest(long long time, QString text);
    void addRequestStart(int id, long long time);
    void addResult(long long time, QString text);
    void addTaskStart(int threadId,long long time);
    void addTaskEnd(int threadId,long long time);
    void addTask(int threadId,long long starttime,long long endtime);
    void addTaskExecute(int threadId,long long starttime,long long endtime);
    void addPeriodicTask(int threadId, char *taskName, long long idate, long long period, long long deadline);
    void registerComputeEngine(int threadId, const QString name);
    void setNbTasks(int nbTasks);
    void logSpecial(int threadId, int what, char *message, long long curTime, long long value);

    void mousePressEvent(QMouseEvent *event);


private:
    QGraphicsScene * scene;
};

#endif // SIMVIEW_H
