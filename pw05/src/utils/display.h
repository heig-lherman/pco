/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Rien à modifier

#ifndef DISPLAY_H
#define DISPLAY_H

#include <QGraphicsView>
#include <QGraphicsItem>
#include <pcosynchro/pcosemaphore.h>
#include <QLabel>
#include <QMovie>


class ActorItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(qreal zValue READ zValue WRITE setZValue)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    ActorItem();
    PcoSemaphore sem;
};


class ClientItem :  public ActorItem
{
public:
    ClientItem();
    unsigned int id;
};
ClientItem *getClient(unsigned int clientId);

class BarberItem :  public ActorItem
{
public:
    BarberItem();
};
BarberItem *getBarber();

class SeatItem :  public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    SeatItem();
    PcoSemaphore sem;
};

class SalonDisplay : public QGraphicsView
{
    Q_OBJECT
public:
    SalonDisplay(unsigned int nbClients, unsigned int nbPlace, QWidget *parent=0);
    ~SalonDisplay();
    unsigned int m_nbPlaces;
    ClientItem *getClient(unsigned int clientId);
    BarberItem *getBarber();



private:
    QGraphicsScene *m_scene;
    QList<ClientItem *> m_clients;
    BarberItem *m_barber;
    SeatItem *m_workchair;

    BarberItem* m_zzz;

    QList<SeatItem *> m_seats;
    unsigned current_client_id = 0;

    void unlockActor(ActorItem *actor);

public slots:
      void barberCuttingHair();
      void barberGoToSleep();

      void clientSitOnChair(unsigned id, int chair_nb);
      void clientSitOnWorkChair(unsigned id);
      void clientHairCutEnded(unsigned id);
      void clientWaitHairToGrow(unsigned id, bool comeBack);
      void clientHairGrow();
      void clientAwayGrowinghair();

      void clientWalkAround(unsigned id);
      void clientWakeUpBarber();
      void clientWaitHairCut(unsigned id);
      void clientAccessEntrance(unsigned id, unsigned ms = 0);

      void hideBarber();
      void finishedAnimation();
      void finishedGroupedAnimation();
      void clientSitOnChair();
      void clientsAccessEntranceFinish();
      void barberCuttingHairStage1();
      void barberCuttingHairStage2();
};


#endif // DISPLAY_H
