#include "voiedroite.h"

VoieDroite::VoieDroite(qreal longueur)
    :Voie()
{
    setNewPen(COULEUR_DROITE);
    this->longueur = longueur;
    this->orientee = false;
    this->posee = false;
    this->lastDistDel = 1000.0;
}

void VoieDroite::calculerAnglesEtCoordonnees(Voie *v)
{
    int ordreVoieFixe;
    if(v==nullptr)
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
    }
    else
    {
        setAngleDeg(0, normaliserAngle(getAngleDeg(1) + 180.0));
    }

    //calculer position relative de 0 et 1.
    coordonneesLiaison[0]->setX(0.0);
    coordonneesLiaison[0]->setY(0.0);
    coordonneesLiaison[1]->setX(longueur * cos(getAngleRad(1)));
    coordonneesLiaison[1]->setY(- longueur * sin(getAngleRad(1)));

    if(this->contact != nullptr)
        calculerPositionContact();

    orientee = true;

    if(!ordreLiaison[0]->estOrientee())
        ordreLiaison[0]->calculerAnglesEtCoordonnees(this);
    if(!ordreLiaison[1]->estOrientee())
        ordreLiaison[1]->calculerAnglesEtCoordonnees(this);
}

void VoieDroite::calculerPositionContact()
{
    this->contact->setPos(longueur * cos(getAngleRad(1)) / 2.0, longueur * sin(getAngleRad(1)) / 2.0);
    this->contact->setAngle(atan2(- coordonneesLiaison[1]->y(), - coordonneesLiaison[1]->x()) + PI / 2.0);
}

QList<QList<Voie*>*> VoieDroite::explorationContactAContact(Voie* voieAppelante)
{
    QList<QList<Voie*>*> temp;

    if(this->contact == nullptr)
    {
        if(ordreLiaison.key(voieAppelante) == 0)
            temp.append(ordreLiaison.value(1)->explorationContactAContact(this));
        else
            temp.append(ordreLiaison.value(0)->explorationContactAContact(this));

        for(int i=0; i< temp.length(); i++)
        {
            temp.at(i)->prepend(this);
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

qreal VoieDroite::getLongueurAParcourir()
{
    return longueur;
}

Voie* VoieDroite::getVoieSuivante(Voie *voieArrivee)
{
    return ordreLiaison.value((ordreLiaison.key(voieArrivee) +1) % 2);
}

void VoieDroite::avanceLoco(qreal &dist, qreal &/*angle*/, qreal &/*rayon*/, qreal /*angleCumule*/, QPointF posActuelle, Voie *voieSuivante)
{
    qreal xLiaison = getPosAbsLiaison(voieSuivante).x();
    qreal yLiaison = getPosAbsLiaison(voieSuivante).y();
    qreal x = posActuelle.x() - xLiaison;
    qreal y = posActuelle.y() - yLiaison;
    qreal distDel = sqrt((x*x) + (y*y));

    if(distDel < dist)
        dist -= distDel;
    else
        dist = 0.0;


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

void VoieDroite::correctionPosition(qreal deltaX, qreal deltaY, Voie *v)
{
    //Correction...
    if(ordreLiaison.key(v) == 0)
    {
        setPos(this->pos().x() + deltaX, this->pos().y() + deltaY);
        coordonneesLiaison[1]->setX(coordonneesLiaison[1]->x() - deltaX);
        coordonneesLiaison[1]->setY(coordonneesLiaison[1]->y() - deltaY);
    }
    else
    {
        coordonneesLiaison[1]->setX(coordonneesLiaison[1]->x() + deltaX);
        coordonneesLiaison[1]->setY(coordonneesLiaison[1]->y() + deltaY);
    }

//    setAngleRad(0, atan2(coordonneesLiaison[1]->y(), coordonneesLiaison[1]->x()));
//    setAngleRad(1, atan2(-coordonneesLiaison[1]->y(), -coordonneesLiaison[1]->x()));


    if(this->contact != nullptr)
        calculerPositionContact();
}


void VoieDroite::correctionPositionLoco(qreal &x, qreal &y)
{
    QPointF p0(x, y);
    QPointF p1 = *coordonneesLiaison[0];
    QPointF p2 = *coordonneesLiaison[1];

    qreal distP1P0 = sqrt((p1.x()-p0.x())*(p1.x()-p0.x()) + (p1.y()-p0.y())*(p1.y()-p0.y()));
    qreal dx = p1.x()-p2.x();
    qreal dy = p1.y()-p2.y();
    qreal distP1P2 = sqrt(dx*dx + dy*dy);

    qreal add   = (p0.x()-p1.x())*(p2.x()-p1.x())+(p0.y()-p1.y())*(p2.y()-p1.y());
    qreal sqrt1 = sqrt((p0.x()-p1.x())*(p0.x()-p1.x())+(p0.y()-p1.y())*(p0.y()-p1.y()));
    qreal sqrt2 = sqrt((p2.x()-p1.x())*(p2.x()-p1.x())+(p2.y()-p1.y())*(p2.y()-p1.y()));
    qreal angleP2P1P0 = add/(sqrt1*sqrt2);

    // This situation can happen if two points are equal, for instance if
    // (x,y) are just exactly on a coordonneesLiaison. In that case, no
    // need to adjust, so that's all right
    if (isnan(angleP2P1P0)) {
        return;
    }

    qreal distP1P3 = distP1P0*angleP2P1P0;
    qreal rapport = distP1P3/distP1P2;

    qreal dxP0 = rapport*dx;
    qreal dyP0 = rapport*dy;

    x = -dxP0;
    y = -dyP0;
}


#define min(a,b) ((a<b)?(a):(b))

QRectF VoieDroite::boundingRect() const
{
    QRectF rect(min(coordonneesLiaison[0]->x(),coordonneesLiaison[1]->x()),
                min(coordonneesLiaison[0]->y(),coordonneesLiaison[1]->y()),
                fabs(coordonneesLiaison[0]->x()-coordonneesLiaison[1]->x()),
                fabs(coordonneesLiaison[0]->y()-coordonneesLiaison[1]->y()));
    rect.adjust(-10,-10,10,10);
    return rect;
}

void VoieDroite::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    painter->setPen(this->pen());
    painter->drawLine(*coordonneesLiaison[0], *coordonneesLiaison[1]);

    drawBoundingRect(painter);
}

void VoieDroite::setEtat(int /*nouvelEtat*/)
{
    qDebug() << "Appel de setEtat sur une voie non variable.";
}
