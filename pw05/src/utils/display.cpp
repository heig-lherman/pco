/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Rien à modifier

#include "display.h"

#include <QPaintEvent>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QEventLoop>
#include <QRandomGenerator>

#include <pcosynchro/pcomutex.h>
#include <QGraphicsPixmapItem>
#include <QDebug>

#include <QVector3D>

#include <iostream>
#include <QThread>

constexpr unsigned NB_CLIENTS_ICONS = 9;

constexpr double SCENE_HEIGHT = 300.0;
constexpr double SCENE_WIDTH = 700.0;
constexpr double SCENE_OFFSET = 50.0;

constexpr double SEAT_WIDTH = 44.0;
constexpr double SEAT_HEIGHT = 52.0;
constexpr double CLIENT_WIDTH = 60.0;
constexpr double CLIENT_HEIGHT = 60.0;
constexpr double BARBER_WIDTH = 100.0;
constexpr double WORKING_CHAIR_WIDTH = 60.0;

constexpr double ENTRANCE_X = 470.0;
constexpr double ENTRANCE_Y = 230.0;

constexpr double SPACE_BETWEEN_SEATS = 140.0;

static SalonDisplay *theDisplay;

ActorItem::ActorItem() = default;
ClientItem::ClientItem() = default;
BarberItem::BarberItem() = default;
SeatItem::SeatItem() = default;

ClientItem *getClient(unsigned int id)
{
    return theDisplay->getClient(id);
}

BarberItem *getBarber()
{
    return theDisplay->getBarber();
}

SalonDisplay::SalonDisplay(unsigned int nbClients, unsigned int nbPlace, QWidget *parent):
    QGraphicsView(parent)
{

    theDisplay = this;

    m_scene=new QGraphicsScene(this);
    this->setRenderHints(QPainter::Antialiasing |
                         QPainter::SmoothPixmapTransform);
    this->setMinimumHeight(SCENE_OFFSET+SCENE_HEIGHT);
    this->setMinimumWidth(SCENE_OFFSET+SCENE_WIDTH);
    this->setScene(m_scene);


    QGraphicsPixmapItem *outdoor_img = m_scene->addPixmap(QPixmap("assets/outside.png") );
    outdoor_img->setPos(SCENE_OFFSET, SCENE_OFFSET);
    outdoor_img->setZValue(0);

    QGraphicsPixmapItem *salon_img = m_scene->addPixmap(QPixmap("assets/salon_2.png") );
    salon_img->setPos(SCENE_OFFSET, SCENE_OFFSET);
    salon_img->setZValue(2);

    // Placement des sièges nbPlace
    for (unsigned i = 0; i < nbPlace; ++i) {
        QPixmap img(QString("assets/waiting_chair.png"));
        QPixmap seatPixmap;
        seatPixmap = img.scaledToWidth(SEAT_WIDTH);
        auto seat = new SeatItem();
        seat->setPixmap(seatPixmap);
        m_scene->addItem(seat);
        m_seats.append(seat);
        unsigned x = ((SCENE_WIDTH / 4 )* 3) - (SEAT_WIDTH * (i / 2)) + (i % 2 * SEAT_WIDTH * 1.5);
        unsigned y = (ENTRANCE_Y + 35) + (SEAT_HEIGHT / 3) * (i / 2);

        seat->setPos(x, y);
        seat->setZValue((i / 2) + 3);
    }

    {
        QPixmap img("assets/barber.png");
        QPixmap barberPixmap;
        barberPixmap = img.scaledToWidth(BARBER_WIDTH);
        m_barber = new BarberItem();
        m_barber->setPixmap(barberPixmap);
        m_scene->addItem(m_barber);

        unsigned x = SCENE_WIDTH / 5 + SEAT_WIDTH + 10;
        unsigned y = SCENE_HEIGHT / 6 * 4 + 10;

        getBarber()->setPos(x, y);
        m_barber->setZValue(3);
    }

    {
        QPixmap img("assets/working_chair.png");
        QPixmap workchairPixmap;
        workchairPixmap = img.scaledToWidth(WORKING_CHAIR_WIDTH);
        m_workchair = new SeatItem();
        m_workchair->setPixmap(workchairPixmap);
        m_scene->addItem(m_workchair);

        unsigned x = SCENE_WIDTH / 4;
        unsigned y = SCENE_HEIGHT / 6 * 5;

        m_workchair->setPos(x, y);
        m_workchair->setZValue(4);
    }


    // Création des clients + placement
    for (unsigned i = 0; i < nbClients; ++i) {
        QPixmap img(QString("assets/actors/p%1L.png").arg(m_clients.size() % NB_CLIENTS_ICONS));

        QPixmap clientPixmap;
        clientPixmap = img.scaledToWidth(CLIENT_WIDTH);
        auto client = new ClientItem();
        client->id = i;
        client->setPixmap(clientPixmap);
        m_scene->addItem(client);
        m_clients.append(client);

        auto r = static_cast<unsigned>(rand());
        unsigned x = r % unsigned(((SCENE_WIDTH)));
        unsigned y = SCENE_HEIGHT / 4 * 3;

        client->setPos(x, y);
        client->setZValue(1);
    }

    // Ajout du Zzz

    {
        QPixmap img("assets/sleeping.png");
        QPixmap zzzPixmap;
        zzzPixmap = img.scaledToWidth(WORKING_CHAIR_WIDTH);

        m_zzz = new BarberItem();
        m_zzz->setPixmap(zzzPixmap);
        m_scene->addItem(m_zzz);

        unsigned x = 0 + WORKING_CHAIR_WIDTH;
        unsigned y = SCENE_HEIGHT / 6 * 3;

        m_zzz->setPos(x, y);
        m_zzz->setZValue(4);

        m_zzz->hide();

    }

    std::cout << "ended client creation\n" ;
}

SalonDisplay::~SalonDisplay()
{
    while (!m_clients.empty()) {
        ClientItem *p = m_clients.at(0);
        p->sem.release();
        m_clients.removeFirst();
        delete p;
    }
    m_barber->sem.release();
    delete m_barber;
}

ClientItem *SalonDisplay::getClient(unsigned int clientId)
{
    return m_clients.at(clientId);
}

BarberItem *SalonDisplay::getBarber()
{
    return m_barber;
}

void SalonDisplay::clientAccessEntrance(unsigned id, unsigned ms)
{
    auto animation1 = new QPropertyAnimation(m_clients.at(id), "pos");
    auto duration = ms ? ms : (rand() % 1000) + 1500;
    animation1->setDuration(duration);
    animation1->setEndValue(QPoint(ENTRANCE_X, ENTRANCE_Y));
    animation1->start();
    QObject::connect(animation1, SIGNAL(finished()), this, SLOT(clientsAccessEntranceFinish()));
}

void SalonDisplay::clientWakeUpBarber(){
    auto animation = new QPropertyAnimation(getBarber(), "pos");
    animation->setDuration(2000);
    animation->setEndValue(QPoint(SCENE_WIDTH / 5 + SEAT_WIDTH + 10, SCENE_HEIGHT / 6 * 4 + 10));
    animation->start();

    getBarber()->show();
    m_zzz->hide();

    QObject::connect(animation, SIGNAL(finished()), this, SLOT(finishedAnimation()));
}

void SalonDisplay::clientSitOnChair(unsigned id, int nb_chair){
    m_clients.at(id)->setZValue(m_seats.at(nb_chair)->zValue() );
    auto pos = m_seats.at(nb_chair)->pos();
    pos.setY(pos.y() - 25);
    pos.setX(pos.x() - 5);
    auto animation = new QPropertyAnimation(m_clients.at(id), "pos");
    animation->setDuration(1500);
    animation->setEndValue(pos);
    animation->start();
}


void SalonDisplay::clientSitOnWorkChair(unsigned id)
{
    current_client_id = id;
    auto animation = new QPropertyAnimation(m_clients.at(id), "pos");
    animation->setDuration(1500);
    animation->setEndValue(QPoint(m_workchair->x() + 100, m_workchair->y()));
    animation->start();
    QObject::connect(animation, SIGNAL(finished()), this, SLOT(clientSitOnChair()));
}

void SalonDisplay::clientWaitHairToGrow(unsigned id, bool comeBack)
{
    m_clients.at(id)->setZValue(1);
    auto r = rand() % 2;
    auto pos = r ? QPoint(SCENE_WIDTH, m_clients.at(id)->y()) : QPoint(200, m_clients.at(id)->y());
    auto animation = new QPropertyAnimation(m_clients.at(id), "pos");
    animation->setDuration(3000);
    animation->setEndValue(pos);
    animation->start();

    if(comeBack){
        QObject::connect(animation, SIGNAL(finished()), this, SLOT(clientAwayGrowinghair()));
    } else {
        QObject::connect(animation, SIGNAL(finished()), this, SLOT(finishedAnimation()));
    }
}

void SalonDisplay::clientAwayGrowinghair()
{
    auto anim = dynamic_cast<QPropertyAnimation*>(sender());
    auto client = dynamic_cast<ClientItem*>(anim->targetObject());
    client->hide();
    auto animation = new QPropertyAnimation(client, "pos");
    animation->setDuration(3000);
    animation->setEndValue(client->pos());
    animation->start();
    QObject::connect(animation, SIGNAL(finished()), this, SLOT(clientHairGrow()));
}


void SalonDisplay::clientHairGrow()
{
    auto anim = dynamic_cast<QPropertyAnimation*>(sender());
    auto client = dynamic_cast<ClientItem*>(anim->targetObject());
    QPixmap img(QString("assets/actors/p%1L.png").arg(client->id % NB_CLIENTS_ICONS));
    QPixmap clientPixmap;
    client->show();
    clientPixmap = img.scaledToWidth(CLIENT_WIDTH);
    client->setPixmap(clientPixmap);
    client->sem.release();
}

void SalonDisplay::clientWalkAround(unsigned id)
{
    m_clients.at(id)->setZValue(1);
    auto animation = new QPropertyAnimation(m_clients.at(id), "pos");
    animation->setDuration(1500);
    auto r = static_cast<unsigned>(rand());
    unsigned x = ((r % 300) - 150) + m_clients.at(id)->pos().x();
    x = x < SCENE_WIDTH / 2 ? SCENE_WIDTH / 2 : x;
    x = x > SCENE_WIDTH ? SCENE_WIDTH : x;
    unsigned y = SCENE_HEIGHT / 4 * 3 + 15 - (r % 20);

    animation->setEndValue(QPoint(x, y));
    animation->start();
    QObject::connect(animation, SIGNAL(finished()), this, SLOT(finishedAnimation()));
}


void SalonDisplay::finishedGroupedAnimation()
{
    auto group = dynamic_cast<QAnimationGroup*>(sender());
    auto animation = dynamic_cast<QPropertyAnimation*>(group->animationAt(0));
    auto actor = dynamic_cast<ActorItem*>(animation->targetObject());
    unlockActor(actor);
    // Comprend pas pourquoi ça bug...
    //delete group;
}

void SalonDisplay::clientSitOnChair()
{

    auto anim = dynamic_cast<QPropertyAnimation*>(sender());
    auto actor = dynamic_cast<ActorItem*>(anim->targetObject());
    actor->setZValue(10);

    auto animation = new QPropertyAnimation(actor, "pos");
    animation->setDuration(500);
    animation->setEndValue(QPoint(m_workchair->x(), m_workchair->y() - 20));
    animation->start();
    QObject::connect(animation, SIGNAL(finished()), this, SLOT(finishedAnimation()));
}

void SalonDisplay::clientsAccessEntranceFinish()
{
    auto animation = dynamic_cast<QPropertyAnimation*>(sender());
    auto actor = dynamic_cast<ActorItem*>(animation->targetObject());
    actor->setZValue(3);
    unlockActor(actor);
    delete animation;
}

void SalonDisplay::hideBarber(){
    auto animation = dynamic_cast<QPropertyAnimation*>(sender());
    delete animation;

    getBarber()->hide();
    m_zzz->show();
}

void SalonDisplay::finishedAnimation()
{
    auto animation = dynamic_cast<QPropertyAnimation*>(sender());
    auto actor = dynamic_cast<ActorItem*>(animation->targetObject());
    unlockActor(actor);
    delete animation;
}

void SalonDisplay::unlockActor(ActorItem *actor)
{
    if (actor) {
        actor->sem.release();
    }
}

void SalonDisplay::clientWaitHairCut(unsigned id){}
void SalonDisplay::clientHairCutEnded(unsigned id){}


void SalonDisplay::barberGoToSleep(){
    auto animation = new QPropertyAnimation(getBarber(), "pos");
    animation->setDuration(2000);
    animation->setEndValue(QPoint(0, getBarber()->pos().y()));
    animation->start();

    QObject::connect(animation, SIGNAL(finished()), this, SLOT(hideBarber()));
}

void SalonDisplay::barberCuttingHair()
{
    auto animation = new QPropertyAnimation(getBarber(), "pos");
    animation->setDuration(500);
    animation->setEndValue(getBarber()->pos());
    animation->start();
    QObject::connect(animation, SIGNAL(finished()), this, SLOT(barberCuttingHairStage1()));
}

void SalonDisplay::barberCuttingHairStage1()
{
    QPixmap imgM(QString("assets/actors/p%1M.png").arg(current_client_id % NB_CLIENTS_ICONS));
    QPixmap clientPixmapM;
    clientPixmapM = imgM.scaledToWidth(CLIENT_WIDTH);
    m_clients.at(current_client_id)->setPixmap(clientPixmapM);
    auto animation = new QPropertyAnimation(getBarber(), "pos");
    animation->setDuration(500);
    animation->setEndValue(getBarber()->pos());
    animation->start();
    QObject::connect(animation, SIGNAL(finished()), this, SLOT(barberCuttingHairStage2()));
}

void SalonDisplay::barberCuttingHairStage2()
{
    QPixmap imgM(QString("assets/actors/p%1S.png").arg(current_client_id % NB_CLIENTS_ICONS));
    QPixmap clientPixmapM;
    clientPixmapM = imgM.scaledToWidth(CLIENT_WIDTH);
    m_clients.at(current_client_id)->setPixmap(clientPixmapM);
    auto animation = new QPropertyAnimation(getBarber(), "pos");
    animation->setDuration(500);
    animation->setEndValue(getBarber()->pos());
    animation->start();
    QObject::connect(animation, SIGNAL(finished()), this, SLOT(finishedAnimation()));
}
