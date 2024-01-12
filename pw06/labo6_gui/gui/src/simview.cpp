#include "simview.h"

#include <QGraphicsTextItem>

#include <QGraphicsRectItem>
#include <QMouseEvent>

#include "arrowitem.h"
#include "guiinterface.h"

#define SLOTWIDTH 2
#define SLOTDIFF 50
#define SLOTHEIGHT 30

#define Z_START 10.0
#define Z_END   10.0
#define Z_EXECUTE 9.0
#define Z_SHOULDEXECUTE 8.0
#define Z_REQUEST 11.0
#define Z_EVENTS  12.0

#define MAXTASKS 100

int tasks[MAXTASKS];

#define NITEMS 1000

#define DIVFACTOR ((long long)(100000000))

SimView::SimView(const std::shared_ptr<ComputationManager>& computationManager, QWidget */*parent*/)
    : QGraphicsView(), computationManager(computationManager)
{
    nbTasks=0;
    nextTime=0;
    for(int i=0;i<MAXTASKS;i++)
        tasks[i]=0;
    scene = new QGraphicsScene();
    this->setScene(scene);
    this->setRenderHints(QPainter::Antialiasing);

    {
        QGraphicsSimpleTextItem *item;
        for(int i=0;i<NITEMS;i++) {
            item=new QGraphicsSimpleTextItem();
            item->hide();
            scene->addItem(item);
            textItemsList.append(item);
        }
    }
    {
        QGraphicsLineItem *item;
        for(int i=0;i<NITEMS;i++) {
            item=new QGraphicsLineItem();
            item->hide();
            scene->addItem(item);
            lineItemsList.append(item);
        }
    }
    {
        QGraphicsRectItem *item;
        for(int i=0;i<NITEMS;i++) {
            item=new QGraphicsRectItem();
            item->hide();
            scene->addItem(item);
            rectItemsList.append(item);
        }
    }

    // TODO : Memory ?
    QGraphicsTextItem *item=scene->addText(QString("Compute Requests"));
    item->setPos(-10-item->boundingRect().width(),30-2*SLOTDIFF);
    item=scene->addText(QString("Results (Green), Aborts (Grey)"));
    item->setPos(-10-item->boundingRect().width(),30-1*SLOTDIFF);
}

void SimView::setNbTasks(int nbTasks)
{
    this->nbTasks=nbTasks;
}

#include <iostream>

void SimView::mousePressEvent(QMouseEvent *event)
  {
      if (QGraphicsItem *item = itemAt(event->pos())) {
          ComputeRequestItem *computeRequest = dynamic_cast<ComputeRequestItem*>(item);
          if (computeRequest) {
              auto id = computeRequest->id;
              computeRequest->setBrush(QColor(60,60,60));
              computationManager->abortComputation(id);
              GuiInterface::instance->logMessage(-1, QString("Asked to abort computation with Id: %1").arg(id));
              auto time = GuiInterface::instance->getCurrentTime();
              int t = (int)(time/DIVFACTOR);
              QGraphicsTextItem *textItem=scene->addText(QString("%1").arg(id));
              textItem->setPos(t*SLOTWIDTH-2, 30-1*SLOTDIFF+SLOTDIFF/2);
              textItem->setZValue(Z_START+10.0);
              QGraphicsRectItem *item=addQGraphicsRectItem(t*SLOTWIDTH,30-SLOTDIFF, SLOTWIDTH, SLOTHEIGHT);
              item->setBrush(QBrush(QColor(128,128,128)));
              item->setPen(Qt::NoPen);
              item->setZValue(Z_START);
              item->show();
          }
      }
  }



QGraphicsSimpleTextItem *SimView::addQGraphicsSimpleTextItem(QString text)
{
    QGraphicsSimpleTextItem *item;
    if (textItemsList.isEmpty()) {
        item=new QGraphicsSimpleTextItem();
        scene->addItem(item);
    }
    else
        item=textItemsList.takeFirst();
    item->setText(text);
    item->show();
    return item;
}

QGraphicsRectItem *SimView::addQGraphicsRectItem(qreal x, qreal y, qreal width, qreal height)
{
    QGraphicsRectItem *item;
    if (textItemsList.isEmpty()) {
        item=new QGraphicsRectItem();
        scene->addItem(item);
    }
    else
        item=rectItemsList.takeFirst();
    item->setRect(x,y,width,height);
    item->show();
    return item;
}


QGraphicsLineItem *SimView::addQGraphicsLineItem( qreal x1, qreal y1, qreal x2, qreal y2)
{
    QGraphicsLineItem *item;
    if (textItemsList.isEmpty()) {
        item=new QGraphicsLineItem();
        scene->addItem(item);
    }
    else
        item=lineItemsList.takeFirst();
    item->setLine(x1,y1,x2,y2);
    item->show();
    return item;
}
#include <iostream>
void SimView::checkTask(int /*taskId*/)
{

}

void SimView::redraw()
{
    scene->update(sceneRect());
}


void SimView::zoomIn()
{
    scale(1.1,1.1);
}

void SimView::zoomOut()
{
    scale(1/1.1,1/1.1);
}

void SimView::zoomFit()
{
    fitInView(scene->itemsBoundingRect(),Qt::KeepAspectRatio);
}

void SimView::addThreadTrigger(int threadId,long long time)
{
    checkTask(threadId);
    int t=(int)time/DIVFACTOR;
    QGraphicsRectItem *item=addQGraphicsRectItem(t*SLOTWIDTH,30+threadId*SLOTDIFF,SLOTWIDTH,SLOTHEIGHT);
    item->setBrush(QBrush(QColor(0,0,255)));
    item->setPen(Qt::NoPen);
    item->show();
}


void SimView::addTask(int threadId,long long starttime,long long endtime)
{
    checkTask(threadId);
    int st=(int)(starttime/DIVFACTOR);
    int et=(int)(endtime/DIVFACTOR);
    {
        QGraphicsRectItem *item=addQGraphicsRectItem(st*SLOTWIDTH,30+threadId*SLOTDIFF,SLOTWIDTH,SLOTHEIGHT);
        item->setBrush(QBrush(QColor(100,200,100)));
        item->setPen(Qt::NoPen);
        item->setZValue(Z_END);
        item->show();
    }
    {
        QGraphicsRectItem *item=addQGraphicsRectItem(et*SLOTWIDTH,30+threadId*SLOTDIFF,SLOTWIDTH,SLOTHEIGHT);
        item->setBrush(QBrush(QColor(255,100,100)));
        item->setPen(Qt::NoPen);
        item->setZValue(Z_END);
        item->show();
    }
    {
        QGraphicsRectItem *item=addQGraphicsRectItem(st*SLOTWIDTH,30+threadId*SLOTDIFF+(SLOTHEIGHT/2-2),(et-st)*SLOTWIDTH,4);
        item->setBrush(QBrush(QColor(Qt::lightGray)));
        item->setPen(Qt::NoPen);
        item->setZValue(Z_SHOULDEXECUTE);
        item->show();
        item->setToolTip(QString("Start time:\t %1\nEnd time:\t %2").arg(starttime).arg(endtime));
    }
    updatePeriodicTasks(endtime);
}

void SimView::addComputeRequest(long long time, QString text)
{
    int t = (int)(time/DIVFACTOR);
    QGraphicsTextItem *textItem=scene->addText(text);
    textItem->setPos(t*SLOTWIDTH-2, 30-2*SLOTDIFF-SLOTDIFF/3-1);
    textItem->setZValue(Z_START+10.0);
    QGraphicsRectItem *item=addQGraphicsRectItem(t*SLOTWIDTH, 30-2*SLOTDIFF, SLOTWIDTH*6, SLOTHEIGHT);
    item->setBrush(QBrush(QColor(128,128,128)));
    item->setPen(Qt::NoPen);
    item->setZValue(Z_START-10.0);
    item->show();
}

void SimView::addRequestStart(int id, long long time)
{
    int t = (int)(time/DIVFACTOR);
    ComputeRequestItem *item;
    item=new ComputeRequestItem();
    item->setId(id);
    QGraphicsTextItem *textItem=scene->addText(QString("%1").arg(id));
    textItem->setPos(t*SLOTWIDTH-2, 30-2*SLOTDIFF+SLOTDIFF/2);
    textItem->setZValue(Z_START+10.0);
    scene->addItem(item);
    item->setRect(t*SLOTWIDTH,30-2*SLOTDIFF, SLOTWIDTH*6, SLOTHEIGHT);
    item->setBrush(QBrush(QColor(100,160,255)));
    item->setPen(Qt::NoPen);
    item->setZValue(Z_START);
    item->show();
    computeRequestList.push_back(item);
}

//ComputeRequestItem* SimView::addComputeRequest(long long time)
//{
//    int t = (int)(time/DIVFACTOR);

//    ComputeRequestItem *item;
//    item=new ComputeRequestItem();
//    scene->addItem(item);
//    item->setRect(t*SLOTWIDTH,30-SLOTDIFF, SLOTWIDTH*6, SLOTHEIGHT);
//    item->setBrush(QBrush(QColor(255,0,0)));
//    item->setPen(Qt::NoPen);
//    item->setZValue(Z_START);
//    item->show();
//    computeRequestList.push_back(item);
//    return item;
//}

void SimView::addResult(long long time, QString text)
{
    int t = (int)(time/DIVFACTOR);
    QGraphicsTextItem *textItem=scene->addText(text);
    textItem->setPos(t*SLOTWIDTH-2, 30-1*SLOTDIFF+SLOTDIFF/2);
    textItem->setZValue(Z_START+10.0);
    QGraphicsRectItem *item=addQGraphicsRectItem(t*SLOTWIDTH,30-SLOTDIFF, SLOTWIDTH, SLOTHEIGHT);
    item->setBrush(QBrush(QColor(0,255,0)));
    item->setPen(Qt::NoPen);
    item->setZValue(Z_START);
    item->show();
}

void SimView::addTaskStart(int threadId,long long time)
{
    checkTask(threadId);
    int t=(int)(time/DIVFACTOR);
    QGraphicsRectItem *item=addQGraphicsRectItem(t*SLOTWIDTH,30+threadId*SLOTDIFF,SLOTWIDTH,SLOTHEIGHT/3);
    item->setBrush(QBrush(QColor(100,200,100)));
    item->setPen(Qt::NoPen);
    item->setZValue(Z_START);
    item->show();
}

void SimView::addTaskEnd(int threadId,long long time)
{
    checkTask(threadId);
    int t=(int)(time/DIVFACTOR);
    QGraphicsRectItem *item=addQGraphicsRectItem(t*SLOTWIDTH,30+threadId*SLOTDIFF+2*(SLOTHEIGHT/3),SLOTWIDTH,SLOTHEIGHT/3);
    item->setBrush(QBrush(QColor(255,100,100)));
    item->setPen(Qt::NoPen);
    item->setZValue(Z_END);
    item->show();
    //updatePeriodicTasks(time);
}

void SimView::addTaskExecute(int threadId,long long starttime,long long endtime)
{
    checkTask(threadId);
    int st=(int)(starttime/DIVFACTOR);
    int et=(int)(endtime/DIVFACTOR);
    QGraphicsRectItem *item=addQGraphicsRectItem((st+1)*SLOTWIDTH,30+threadId*SLOTDIFF+(SLOTHEIGHT/2-4),(et-st)*SLOTWIDTH-SLOTWIDTH,8);
    item->setBrush(QBrush(QColor(Qt::darkGray)));
    item->setPen(Qt::NoPen);
    item->setZValue(Z_EXECUTE);
    item->show();
    updatePeriodicTasks(endtime);
}

void SimView::registerComputeEngine(int threadId, const QString name) {
    //threadNames[threadId] = std::string(name);
    QGraphicsTextItem *item=scene->addText(QString("%1").arg(name));
    item->setPos(-10-item->boundingRect().width(),30+threadId*SLOTDIFF);
}

void SimView::updatePeriodicTasks(long long time)
{
    return;

    // Update the periodic tasks
    foreach (PeriodicTask *task,tasksList)
    {
        while(task->nextReq<time)
        {
            int t=(int)(task->nextReq/DIVFACTOR);
            ArrowItem *item;
            if (task->period == task->deadline)
                item = new ArrowItem(0,3);
            else
                item = new ArrowItem(0,1);
            scene->addItem(item);
            item->setZValue(Z_REQUEST);
            item->setLine(t*SLOTWIDTH,
                          30+task->taskId*SLOTDIFF+SLOTHEIGHT/2,
                          t*SLOTWIDTH,
                          30+task->taskId*SLOTDIFF-10);

            if ((task->period != task->deadline) && (task->deadline != 0))
            {
                // We draw the deadline
                int t=(int)((task->nextReq+task->deadline)/DIVFACTOR);

                ArrowItem *item = new ArrowItem(0,2);
                scene->addItem(item);
                item->setZValue(Z_REQUEST);
                item->setLine(t*SLOTWIDTH,
                              30+task->taskId*SLOTDIFF+SLOTHEIGHT/2,
                              t*SLOTWIDTH,
                              30+task->taskId*SLOTDIFF-10);
            }

//            QGraphicsLineItem *item=addQGraphicsLineItem(t*SLOTWIDTH,30+task->taskId*SLOTDIFF+SLOTHEIGHT/2,t*SLOTWIDTH,30+task->taskId*SLOTDIFF-10);
//            item->setToolTip(QString("Req time:\t %L1").arg(task->nextReq));
//            /*QGraphicsLineItem *item1=*/ addQGraphicsLineItem(t*SLOTWIDTH-5,30+task->taskId*SLOTDIFF-10+5,t*SLOTWIDTH,30+task->taskId*SLOTDIFF-10);
//            /*QGraphicsLineItem *item2=*/ addQGraphicsLineItem(t*SLOTWIDTH+5,30+task->taskId*SLOTDIFF-10+5,t*SLOTWIDTH,30+task->taskId*SLOTDIFF-10);
            task->nextReq+=task->period;
        }
    }

    // Update the time scale
    while(nextTime<time)
    {
        int t=(int)(nextTime/DIVFACTOR);
        int ID=nbTasks+1;
        /*QGraphicsLineItem *item=*/ addQGraphicsLineItem(t*SLOTWIDTH,30+ID*SLOTDIFF+20,t*SLOTWIDTH,30+ID*SLOTDIFF+10);
        QGraphicsSimpleTextItem *textItem=addQGraphicsSimpleTextItem(QString("%1 s").arg(nextTime/((long long)1000000000)));

        textItem->setPos(t*SLOTWIDTH-textItem->boundingRect().width()/2,30+ID*SLOTDIFF-10);
        nextTime+=(long long)1000000000;
    }
}

void SimView::addPeriodicTask(int threadId,char *taskName,long long idate,long long period,long long deadline)
{
    PeriodicTask *task=new PeriodicTask(threadId,taskName,idate,period,deadline);
    if (task->idate==0)
        task->nextReq=0;
    else
        task->nextReq=idate;
    tasksList.append(task);
}

#include "messages.h"
#include "arrowitem.h"
#define ARROWHEIGHT SLOTHEIGHT/2

#include <iostream>

void SimView::logSpecial(int threadId, int what, char *message, long long curTime,long long value)
{
    int t=(int)(curTime/DIVFACTOR);
    switch(what) {
    case MSG_MUTEXACQUIRE:
    case MSG_SEMP:
    case MSG_CONDVARWAIT:
    case MSG_EVENTWAIT:{

        ArrowItem *arrowitem = new ArrowItem(0,2);
        scene->addItem(arrowitem);
        arrowitem->setZValue(Z_EVENTS);
        arrowitem->setLine(t*SLOTWIDTH,
                      30+threadId*SLOTDIFF+SLOTHEIGHT/2,
                      t*SLOTWIDTH-4,
                      30+threadId*SLOTDIFF);

        QString text;
        if (what == MSG_EVENTWAIT)
                text = QString("%1: %2").arg(message).arg(value);
            else
                text = QString("%1").arg(message);
            QGraphicsSimpleTextItem *textItem=addQGraphicsSimpleTextItem(text);

        textItem->setPos(t*SLOTWIDTH-4-textItem->boundingRect().width()/2,30+threadId*SLOTDIFF-textItem->boundingRect().height());

    } break;
    case MSG_MUTEXRELEASE:
    case MSG_SEMV:
    case MSG_CONDVARBROADCAST:
    case MSG_CONDVARSIGNAL:
    case MSG_EVENTSIGNAL: {

        ArrowItem *arrowitem = new ArrowItem(0,1);
        scene->addItem(arrowitem);
        arrowitem->setZValue(Z_EVENTS);
        arrowitem->setLine(t*SLOTWIDTH,
                      30+threadId*SLOTDIFF+SLOTHEIGHT/2,
                      t*SLOTWIDTH+4,
                      30+threadId*SLOTDIFF);

        QString text;
        if (what == MSG_EVENTSIGNAL)
                text = QString("%1: %2").arg(message).arg(value);
            else if (what == MSG_CONDVARBROADCAST)
                text = QString("Br:%1").arg(message);
            else
                text = QString("%1").arg(message);
            QGraphicsSimpleTextItem *textItem=addQGraphicsSimpleTextItem(text);
    
        textItem->setPos(t*SLOTWIDTH+4-textItem->boundingRect().width()/2,30+threadId*SLOTDIFF-textItem->boundingRect().height());

    } break;
    default:
        std::cerr << "Error, no message support for this" << std::endl;
    }
}
