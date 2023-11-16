#include "voiebuttoir.h"

VoieButtoir::VoieButtoir(qreal longueur)
    :Voie()
{
    setNewPen(COULEUR_BUTTOIR);
    this->longueur = longueur;
    this->orientee = false;
    this->posee = false;
}

void VoieButtoir::calculerAnglesEtCoordonnees(Voie *v)
{
    if(v == nullptr)
    {
        setAngleDeg(0, 0.0);
    }
    else
    {
        setAngleDeg(0, normaliserAngle(v->getAngleVoisin(this) + 180.0));
    }

    //calculer position relative de 0 et 1.
    coordonneesLiaison[0]->setX(0.0);
    coordonneesLiaison[0]->setY(0.0);

    if(this->contact != nullptr)
        calculerPositionContact();

    orientee = true;

    if(!ordreLiaison[0]->estOrientee())
        ordreLiaison[0]->calculerAnglesEtCoordonnees(this);
}

void VoieButtoir::calculerPositionContact()
{
    //dummy code. A priori, on ne met pas de contact sur un buttoir.
    this->contact->setPos(0.0,0.0);
}

QList<QList<Voie*>*> VoieButtoir::explorationContactAContact(Voie */*voieAppelante*/)
{
    QList<QList<Voie*>*> temp;

    temp.append(new QList<Voie*>);
    temp.at(0)->append(this);

    return temp;
}

qreal VoieButtoir::getLongueurAParcourir()
{
    return longueur;
}

Voie* VoieButtoir::getVoieSuivante(Voie */*voieArrivee*/)
{
    //A revoir!
    return nullptr;
}

void VoieButtoir::avanceLoco(qreal &dist, qreal &angle, qreal &rayon, qreal /*angleCumule*/, QPointF posActuelle, Voie *voieSuivante)
{
    angle = 0.0;
    rayon = 0.0;

    qreal distanceAuContact = sqrt((posActuelle.x() - this->pos().x()) *
                                   (posActuelle.x() - this->pos().x()) +
                                   (posActuelle.y() - this->pos().y()) *
                                   (posActuelle.y() - this->pos().y()));
    if (distanceAuContact + LONGUEUR_LOCO / 2.0 > longueur)
    {
        //déclarer un deraillement.
    }
    else
    {
        if(voieSuivante == nullptr)
        {
            dist = 0.0;
        }
        else
        {
            if(distanceAuContact < dist)
            {
                dist -= distanceAuContact;
            }
            else
            {
                dist = 0.0;
            }
        }
    }
}



void VoieButtoir::correctionPosition(qreal deltaX, qreal deltaY, Voie */*v*/)
{
    //corrections...
    setPos(this->pos().x() + deltaX, this->pos().y() + deltaY);

    if(this->contact != nullptr)
        calculerPositionContact();
}


void VoieButtoir::correctionPositionLoco(qreal &/*x*/, qreal &/*y*/)
{
    //TODO
}


#define min(a,b) ((a<b)?(a):(b))

QRectF VoieButtoir::boundingRect() const
{
    QPointF temp = QPointF(- longueur * cos(getAngleRad(0)),
                                longueur * sin(getAngleRad(0)));
    QRectF rect(min(coordonneesLiaison[0]->x(),temp.x()),
                min(coordonneesLiaison[0]->y(),temp.y()),
                fabs(coordonneesLiaison[0]->x()-temp.x()),
                fabs(coordonneesLiaison[0]->y()-temp.y()));
    rect.adjust(-10,-10,10,10);
    return rect;
}


void VoieButtoir::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    painter->setPen(this->pen());
    QPointF temp = QPointF(- longueur * cos(getAngleRad(0)),
                                longueur * sin(getAngleRad(0)));

    painter->drawLine(*coordonneesLiaison[0], temp);
    painter->drawEllipse(temp, 5.0, 5.0);
    drawBoundingRect(painter);

}

void VoieButtoir::setEtat(int /*nouvelEtat*/)
{
    qDebug() << "Appel de setEtat sur une voie non variable.";
}
