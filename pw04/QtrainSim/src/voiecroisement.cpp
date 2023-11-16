#include "voiecroisement.h"

VoieCroisement::VoieCroisement(qreal angle, qreal longueur)
    :Voie()
{
    setNewPen(COULEUR_CROISEMENT);
    this->angle = angle;
    this->longueur = longueur;
    this->orientee = false;
    this->posee = false;
    this->lastDistDel = 1000.0;
}

void VoieCroisement::calculerAnglesEtCoordonnees(Voie *v)
{
    int ordreVoieFixe;
    if(v == nullptr)
    {
        ordreVoieFixe = 0;
        setAngleDeg(0, 0.0);
    }
    else
    {
        ordreVoieFixe = ordreLiaison.key(v);
        setAngleDeg(ordreVoieFixe, normaliserAngle(v->getAngleVoisin(this) + 180.0));
    }

    if(ordreVoieFixe == 0)
    {
        setAngleDeg(1, normaliserAngle(getAngleDeg(0) + 180.0));
        setAngleDeg(2, normaliserAngle(getAngleDeg(0) + angle));
        setAngleDeg(3, normaliserAngle(getAngleDeg(2) + 180.0));
    }
    else if(ordreVoieFixe == 1)
    {
        setAngleDeg(0, normaliserAngle(getAngleDeg(1) + 180.0));
        setAngleDeg(2, normaliserAngle(getAngleDeg(0) + angle));
        setAngleDeg(3, normaliserAngle(getAngleDeg(2) + 180.0));
    }
    else if(ordreVoieFixe == 2)
    {
        setAngleDeg(0, normaliserAngle(getAngleDeg(2) - angle));
        setAngleDeg(1, normaliserAngle(getAngleDeg(0) + 180.0));
        setAngleDeg(3, normaliserAngle(getAngleDeg(2) + 180.0));
    }
    else if(ordreVoieFixe == 3)
    {
        setAngleDeg(0, normaliserAngle(getAngleDeg(3) + (180.0 - angle)));
        setAngleDeg(1, normaliserAngle(getAngleDeg(0) + 180.0));
        setAngleDeg(2, normaliserAngle(getAngleDeg(0) + angle));
    }

    //calculer position relative de 0 et 1.
    coordonneesLiaison[0]->setX(0.0);
    coordonneesLiaison[0]->setY(0.0);
    coordonneesLiaison[1]->setX(longueur * cos(getAngleRad(1)));
    coordonneesLiaison[1]->setY(- longueur * sin(getAngleRad(1)));
    coordonneesLiaison[2]->setX((longueur / 2.0) * (cos(getAngleRad(1)) + cos(getAngleRad(2))));
    coordonneesLiaison[2]->setY(-((longueur / 2.0) * (sin(getAngleRad(1)) + sin(getAngleRad(2)))));
    coordonneesLiaison[3]->setX((longueur / 2.0) * (cos(getAngleRad(1)) + cos(getAngleRad(3))));
    coordonneesLiaison[3]->setY(-((longueur / 2.0) * (sin(getAngleRad(1)) + sin(getAngleRad(3)))));

    if(this->contact != nullptr)
        calculerPositionContact();

    orientee = true;

    if(!ordreLiaison[0]->estOrientee())
        ordreLiaison[0]->calculerAnglesEtCoordonnees(this);
    if(!ordreLiaison[1]->estOrientee())
        ordreLiaison[1]->calculerAnglesEtCoordonnees(this);
    if(!ordreLiaison[2]->estOrientee())
        ordreLiaison[2]->calculerAnglesEtCoordonnees(this);
    if(!ordreLiaison[3]->estOrientee())
        ordreLiaison[3]->calculerAnglesEtCoordonnees(this);
}

void VoieCroisement::calculerPositionContact()
{
    //dummy code. A priori, on ne met pas de contact sur un croisement.
    this->contact->setPos(0.0,0.0);
}

QList<QList<Voie*>*> VoieCroisement::explorationContactAContact(Voie* voieAppelante)
{
    QList<QList<Voie*>*> temp;

    if(this->contact == nullptr)
    {
        if(ordreLiaison.key(voieAppelante) == 0)
        {
            temp.append(ordreLiaison[1]->explorationContactAContact(this));
        }
        else if(ordreLiaison.key(voieAppelante) == 1)
        {
            temp.append(ordreLiaison[0]->explorationContactAContact(this));
        }
        else if(ordreLiaison.key(voieAppelante) == 2)
        {
            temp.append(ordreLiaison[3]->explorationContactAContact(this));
        }
        else if(ordreLiaison.key(voieAppelante) == 3)
        {
            temp.append(ordreLiaison[2]->explorationContactAContact(this));
        }

        foreach(QList<Voie*>* lv, temp)
        {
            lv->prepend(this);
        }
    }
    else
    {
        QList<Voie*>* temp2 = new QList<Voie*>();
        temp2->append(this);
        temp.append(temp2);
    }

    return temp;
}

qreal VoieCroisement::getLongueurAParcourir()
{
    return longueur;
}

Voie* VoieCroisement::getVoieSuivante(Voie *voieArrivee)
{
    int ordreVoieArrivee = ordreLiaison.key(voieArrivee);

    if( ordreVoieArrivee == 0)
        return ordreLiaison.value(1);
    else if (ordreVoieArrivee == 1)
        return ordreLiaison.value(0);
    else if (ordreVoieArrivee == 2)
        return ordreLiaison.value(3);
    else
        return ordreLiaison.value(2);
}

void VoieCroisement::avanceLoco(qreal &dist, qreal &angle, qreal &rayon, qreal /*angleCumule*/, QPointF posActuelle, Voie *voieSuivante)
{
    angle = 0.0;
    rayon = 0.0;

    qreal xLiaison = getPosAbsLiaison(voieSuivante).x();
    qreal yLiaison = getPosAbsLiaison(voieSuivante).y();
    qreal x = posActuelle.x() - xLiaison;
    qreal y = posActuelle.y() - yLiaison;
    qreal distDel = sqrt((x*x) + (y*y));

    if(distDel < dist)
    {
        dist -= distDel;
    }
    else
    {
        dist = 0.0;
    }

    if(lastDistDel > distDel)
    {
        lastDistDel = distDel;
    }
    else
    {
        dist = 0.1;
    }
    if(dist > 0.0)
        lastDistDel = 1000.0;
}

void VoieCroisement::correctionPosition(qreal deltaX, qreal deltaY, Voie *v)
{
    //correction
    if(ordreLiaison.key(v) == 0)
    {
        setPos(this->pos().x() + deltaX, this->pos().y() + deltaY);
        coordonneesLiaison[1]->setX(coordonneesLiaison[1]->x() - deltaX);
        coordonneesLiaison[1]->setY(coordonneesLiaison[1]->y() - deltaY);
        coordonneesLiaison[2]->setX(coordonneesLiaison[2]->x() - deltaX);
        coordonneesLiaison[2]->setY(coordonneesLiaison[2]->y() - deltaY);
        coordonneesLiaison[3]->setX(coordonneesLiaison[3]->x() - deltaX);
        coordonneesLiaison[3]->setY(coordonneesLiaison[3]->y() - deltaY);
    }
    else
    {
        coordonneesLiaison[ordreLiaison.key(v)]->setX(coordonneesLiaison[ordreLiaison.key(v)]->x() + deltaX);
        coordonneesLiaison[ordreLiaison.key(v)]->setY(coordonneesLiaison[ordreLiaison.key(v)]->y() + deltaY);
    }

    if(this->contact != nullptr)
        calculerPositionContact();
}


void VoieCroisement::correctionPositionLoco(qreal &/*x*/, qreal &/*y*/)
{
    //TODO
}


QRectF VoieCroisement::boundingRect() const
{
    return QRectF(QPointF(-200.0, -200.0),
                  QPointF(400.0, 400.0));
}

void VoieCroisement::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    painter->setPen(this->pen());
    painter->drawLine(*coordonneesLiaison[0], *coordonneesLiaison[1]);
    painter->drawLine(*coordonneesLiaison[2], *coordonneesLiaison[3]);
    drawBoundingRect(painter);

}

void VoieCroisement::setEtat(int /*nouvelEtat*/)
{
    qDebug() << "Appel de setEtat sur une voie non variable.";
}
