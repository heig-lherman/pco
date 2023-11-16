#include "loco.h"
#include "trainsimsettings.h"

panneauNumLoco::panneauNumLoco(int numLoco, QObject *parent) :
    QObject(parent)
{
    this->numLoco = numLoco;
}

QRectF panneauNumLoco::boundingRect() const
{
    return QRectF(- LARGEUR_LOCO * 0.4, - LARGEUR_LOCO * 0.4, LARGEUR_LOCO * 0.8, LARGEUR_LOCO * 0.8);
}

void panneauNumLoco::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    painter->setPen(Qt::lightGray);
    painter->setBrush(Qt::lightGray);

    painter->drawRoundedRect(boundingRect(), LARGEUR_LOCO * 0.2, LARGEUR_LOCO * 0.2);

    painter->setPen(Qt::black);

    QString t;
    t.setNum(numLoco);

    painter->setFont(QFont("Verdana", 22, 99));

    painter->drawText(QRectF(-LARGEUR_LOCO * 0.45, - LARGEUR_LOCO * 0.45, LARGEUR_LOCO * 0.9, LARGEUR_LOCO * 0.9), t, QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
}

int panneauNumLoco::getNumLoco()
{
    return numLoco;
}

Loco::Loco(int numLoco, QObject *parent) :
    QObject(parent)
{
    this->numLoco1 = new panneauNumLoco(numLoco);
    this->numLoco1->setParentItem(this);
    this->numLoco1->setVisible(true);
    this->numLoco1->setPos(LONGUEUR_LOCO * 0.3, 0.0);
    this->numLoco2 = new panneauNumLoco(numLoco);
    this->numLoco2->setParentItem(this);
    this->numLoco2->setVisible(true);
    this->numLoco2->setPos(- LONGUEUR_LOCO * 0.3, 0.0);
    this->numLoco2->setRotation(this->numLoco2->rotation() + 180.0);
    this->vitesse = 0;
    this->vitesseFuture = 0;
    this->active = true;
    this->direction = DIRECTION_LOCO_GAUCHE;
    this->angleCumule = 0.0;
    this->alerteProximite = false;
    this->inverser = false;
    this->deraille = false;
    this->timer = new QTimer(this);
    this->mutex = new QMutex();
    this->VarCond = new QWaitCondition();
    setZValue(ZVAL_LOCO);

    CONNECT(timer, SIGNAL(timeout()), this, SLOT(adapterVitesse()));
}

void Loco::setVitesse(int v)
{
    if(TrainSimSettings::getInstance()->getInertie())
    {
        this->vitesseFuture = v;
        this->timer->start(INERTIE_LOCO);
    }
    else
    {
        this->vitesse = this->vitesseFuture = v;
    }
}

int Loco::getVitesse()
{
    return this->vitesse;
}

void Loco::setDirection(int d)
{
    this->direction = d;
}

int Loco::getDirection()
{
    return this->direction;
}

void Loco::setCouleur(int r, int g, int b)
{
    couleur = QColor(r,g,b);
}

QColor Loco::getCouleur()
{
    return couleur;
}

QRectF Loco::boundingRect() const
{
    return QRectF(- (LONGUEUR_LOCO / 2.0 + LONGUEUR_FEUX),
                  - (LONGUEUR_LOCO / 2.0 + LONGUEUR_FEUX),
                  LONGUEUR_LOCO + 2.0 * LONGUEUR_FEUX,
                  LONGUEUR_LOCO + 2.0 * LONGUEUR_FEUX);
}

void Loco::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{

    painter->setBrush(couleur);

    painter->drawRect(QRectF(-LONGUEUR_LOCO / 2.0, -LARGEUR_LOCO / 2.0, LONGUEUR_LOCO, LARGEUR_LOCO));

    painter->setPen(Qt::yellow);
    painter->setBrush(Qt::yellow);
    painter->setOpacity(0.65);

    if(direction == DIRECTION_LOCO_DROITE)
    {
        painter->drawPie(QRectF(- LONGUEUR_LOCO / 2.0 - LONGUEUR_FEUX,
                                - 3.0 * LARGEUR_LOCO / 4.0,
                                2.0 * LARGEUR_LOCO,
                                2.0 * LARGEUR_LOCO),
                         170*16,
                         20 *16);
        painter->drawPie(QRectF(- LONGUEUR_LOCO / 2.0 - LONGUEUR_FEUX,
                                - 5.0 * LARGEUR_LOCO / 4.0,
                                2.0 * LARGEUR_LOCO,
                                2.0 * LARGEUR_LOCO),
                         170*16,
                         20 *16);
    }
    else
    {
        painter->drawPie(QRectF(LONGUEUR_LOCO / 2.0 - LONGUEUR_FEUX,
                                - 3.0 * LARGEUR_LOCO / 4.0,
                                2.0 * LARGEUR_LOCO,
                                2.0 * LARGEUR_LOCO),
                         350*16,
                         20 *16);
        painter->drawPie(QRectF(LONGUEUR_LOCO / 2.0 - LONGUEUR_FEUX,
                                - 5.0 * LARGEUR_LOCO / 4.0,
                                2.0 * LARGEUR_LOCO,
                                2.0 * LARGEUR_LOCO),
                         350*16,
                         20 *16);
    }

    if(alerteProximite)
    {
        painter->setPen(Qt::red);
        painter->setBrush(QBrush());

        painter->drawEllipse(QPointF(0.0, 0.0),LONGUEUR_LOCO / 2.0, LONGUEUR_LOCO / 2.0);
        painter->drawEllipse(QPointF(0.0, 0.0),LONGUEUR_LOCO / 2.0 -6, LONGUEUR_LOCO / 2.0 -6);
        painter->drawEllipse(QPointF(0.0, 0.0),LONGUEUR_LOCO / 2.0 -12, LONGUEUR_LOCO / 2.0 -12);
    }
}

void Loco::setVoie(Voie *v)
{
    this->voieActuelle = v;
}

Voie* Loco::getVoie()
{
    return this->voieActuelle;
}

void Loco::setVoieSuivante(Voie *v)
{
    this->voieSuivante = v;
}

Voie* Loco::getVoieSuivante()
{
    return this->voieSuivante;
}

void Loco::setActive(bool active)
{
    this->active = active;
}

bool Loco::getActive()
{
    return active;
}

#include <iostream>
#include "mainwindow.h"

#ifdef FULLCHECK
void CHECK(bool condition)
{
    if (!condition) {
        std::cout << "Aie, that's bad" << std::endl;
    }
}
#else // FULLCHECK
void CHECK(bool /*condition*/) {}
#endif // FULLCHECK

void Loco::avanceDUneVoie()
{
    CHECK(voieActuelle != nullptr);
    Voie* viensDe = voieActuelle;

    CHECK(voieSuivante != nullptr);
    voieActuelle = voieSuivante;

    voieSuivante = voieActuelle->getVoieSuivante(viensDe);
    CHECK(voieSuivante != nullptr);

    setPos(voieActuelle->getPosAbsLiaison(viensDe));

    corrigerAngle(voieActuelle->getNouvelAngle(viensDe));

    if(voieActuelle->getContact() != nullptr)
    {
        Contact* ctc1 = voieActuelle->getContact();
        Contact* ctc2 = nullptr;
        viensDe = voieActuelle;
        Voie* v1 = voieSuivante;
        Voie* v2 = v1->getVoieSuivante(viensDe);

        CHECK(v2 != nullptr);

        while (ctc2 == nullptr)
        {
            ctc2 = v1->getContact();
            viensDe = v1;
            v1 = v2;
            v2 = v1->getVoieSuivante(viensDe);
            CHECK(v2 != nullptr);
        }

        nouveauSegment(ctc1, ctc2, this);

        voieActuelle->getContact()->active(); //pas ideal... A revoir.
        if (TrainSimSettings::getInstance()->getViewLocoLog())
        {
            this->controller->console->append(QString("# Passe le contact numéro %1").arg(voieActuelle->getContact()->getNumContact()));
            std::cout << "Loco " << this->numLoco1->getNumLoco() << " : Passe le contact " << voieActuelle->getContact()->getNumContact() << std::endl;
        }
    }
}

void Loco::avancer(qreal distance)
{
    qreal dist = distance;
    qreal angle = 0.0;
    qreal rayon = 0.0;

    while(true)
    {
        this->voieActuelle->avanceLoco(dist, angle, rayon, this->angleCumule, this->pos(), this->voieSuivante);

        if(rayon == 0.0)
        {
            avancerDroit(distance - dist);
        }
        else
        {
            avancerCourbe(-angle, rayon);
            angleCumule += angle;
            if(angleCumule < 0.0)
                angleCumule += 360.0;
            if(angleCumule > 360.0)
                angleCumule -= 360.0;
        }

        if(dist == 0.0)
        {
            break;
        }
        avanceDUneVoie();
    }
}

void Loco::avancerDroit(qreal distance)
{
    qreal x =  distance * cos(angleCumule * (PI / 180.0));
    qreal y = -distance * sin(angleCumule * (PI / 180.0));
    CHECK(!isnan(x));
    CHECK(!isnan(y));
    voieActuelle->correctionPositionLoco(x, y);
    CHECK(!isnan(x));
    CHECK(!isnan(y));
    moveBy(x,y);

}

void Loco::avancerCourbe(qreal angle, qreal rayon)
{
    qreal angleAbs = angle < 0.0 ? -angle : angle;
    qreal dist = rayon * tan(angleAbs * PI / 360.0);
    avancerDroit(dist);
    setRotation(rotation() + angle);
    avancerDroit(dist);
}

void Loco::setAngleCumule(qreal a)
{
    this->angleCumule = a;
}

qreal Loco::getAngleCumule()
{
    return this->angleCumule;
}

void Loco::setSegmentActuel(Segment *s)
{
    this->segmentActuel = s;
}

void Loco::setAlerteProximite(bool b)
{
    this->alerteProximite = b;
}

bool Loco::getAlerteProximite()
{
    return this->alerteProximite;
}

QPolygonF Loco::getContour()
{
    return mapToScene(QRectF(-LONGUEUR_LOCO / 2.0, -LARGEUR_LOCO / 2.0, LONGUEUR_LOCO, LARGEUR_LOCO));
}

void Loco::inverserSens()
{
    if(TrainSimSettings::getInstance()->getInertie())
    {
        inverser = true;
        this->timer->start(INERTIE_LOCO);
    }
    else
    {
        this->setRotation(this->rotation() + 180.0);
        Voie* viensDe = voieSuivante;
        voieSuivante = voieActuelle->getVoieSuivante(viensDe);
        CHECK(voieSuivante != nullptr);
        this->angleCumule -= 180.0;
    }
}

void Loco::corrigerAngle(qreal nouvelAngle)
{
    QPointF avantLoco = mapToScene(LONGUEUR_LOCO / 2.0, 0.0);
    qreal angleReel = atan2(- avantLoco.y() + pos().y(), avantLoco.x() - pos().x()) * 180.0 / PI;

    setRotation(rotation() + angleReel);

    setRotation( rotation() - nouvelAngle);

    this->angleCumule = nouvelAngle;
}

void Loco::locoSurSegment(Segment *s)
{
    if(s == segmentActuel)
    {
        //Alerte collision!!!
        //l'alerte proximité a été implémentée de manière differente...
    }
}

void Loco::voieVariableModifiee(Voie *v)
{
    if(v == voieActuelle)
    {
        deraille = true;
        vitesse = vitesseFuture = 0;
        setRotation(rotation()+20.0);
    }
}

void Loco::adapterVitesse()
{
    if(inverser)
    {
        if (vitesse!=0)
            vitesse--;
        if(vitesse ==0)
        {
            this->setRotation(rotation()+180.0);
            Voie* viensDe = voieSuivante;
            CHECK(viensDe != nullptr);
            voieSuivante = voieActuelle->getVoieSuivante(viensDe);
            CHECK(voieSuivante != nullptr);
            this->angleCumule -= 180.0;
            inverser = false;
        }
    }
    else
    {
        if(vitesse - vitesseFuture < 0)
            vitesse++;
        else if(vitesse - vitesseFuture > 0)
            vitesse--;
        else
            timer->stop();
    }
}
