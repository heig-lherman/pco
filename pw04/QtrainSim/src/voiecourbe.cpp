#include "voiecourbe.h"

VoieCourbe::VoieCourbe(qreal angle, qreal rayon, int direction)
    :Voie()
{
    setNewPen(COULEUR_COURBE);
    this->rayon = rayon;
    this->angle = angle;
    this->direction = direction;
    this->orientee = false;
    this->posee = false;
    this->lastDistDel = 1000.0;
}

void VoieCourbe::calculerAnglesEtCoordonnees(Voie *v)
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
        setAngleDeg(1, normaliserAngle(getAngleDeg(0) - 180.0 + direction * angle));
    }
    else
    {
        setAngleDeg(0, normaliserAngle(getAngleDeg(1) - 180.0 - direction * angle));
    }

    //calculer coordonnees du centre.
    centre.setX(rayon * cos(getAngleRad(0) - (direction / 2.0) * PI));
    centre.setY(- rayon * sin(getAngleRad(0) - (direction / 2.0) * PI));

    //calculer position relative de 0 et 1.
    coordonneesLiaison[0]->setX(0.0);
    coordonneesLiaison[0]->setY(0.0);
    coordonneesLiaison[1]->setX(centre.x() + rayon * cos(getAngleRad(1) - (direction / 2.0) * PI));
    coordonneesLiaison[1]->setY(centre.y() - rayon * sin(getAngleRad(1) - (direction / 2.0) * PI));

    if(this->contact != nullptr)
    {
        calculerPositionContact();
    }

    orientee = true;

    if(!ordreLiaison[0]->estOrientee())
        ordreLiaison[0]->calculerAnglesEtCoordonnees(this);
    if(!ordreLiaison[1]->estOrientee())
        ordreLiaison[1]->calculerAnglesEtCoordonnees(this);
}

void VoieCourbe::calculerPositionContact()
{
    this->contact->setPos(centre.x() + rayon * cos(getAngleRad(1) - direction * (PI + angle * PI / 180.0) / 2.0),
                          centre.y() - rayon * sin(getAngleRad(1) - direction * (PI + angle * PI / 180.0) / 2.0));
    this->contact->setAngle(atan2(- coordonneesLiaison[1]->y(), - coordonneesLiaison[1]->x()) + direction * PI / 2.0);
}

QList<QList<Voie*>*> VoieCourbe::explorationContactAContact(Voie* voieAppelante)
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

qreal VoieCourbe::getLongueurAParcourir()
{
    return 2.0 * (angle * PI / 180.0) * rayon;
}

Voie* VoieCourbe::getVoieSuivante(Voie *voieArrivee)
{
    return ordreLiaison.value((ordreLiaison.key(voieArrivee) +1) % 2);
}

void VoieCourbe::avanceLoco(qreal &dist, qreal &angle, qreal &rayon, qreal angleCumule, QPointF posActuelle, Voie *voieSuivante)
{
    rayon = this->rayon;

    qreal angleAParcourir = (dist / this->rayon) * (180.0 / PI);

    qreal angleRestant = (getAngleDeg(ordreLiaison.key(voieSuivante)) + 360.0) - (angleCumule + 360.0);

    while(angleRestant < - this->angle * 2.0)
    {
        angleRestant += 360.0;
    }
    while(angleRestant > this->angle * 2.0)
    {
        angleRestant -= 360.0;
    }

    if(angleRestant < 0.0)
    {
        if(- angleRestant < angleAParcourir)
        {
            dist -= - angleRestant * (PI / 180.0) * rayon;
            angle = angleRestant;
        }
        else
        {
            dist = 0.0;
            angle = - angleAParcourir;
        }
    }
    else
    {
        if(angleRestant < angleAParcourir)
        {
            dist -= angleRestant * (PI / 180.0) * rayon;
            angle = angleRestant;
        }
        else
        {
            dist = 0.0;
            angle = angleAParcourir;
        }
    }

    qreal xLiaison = getPosAbsLiaison(voieSuivante).x();
    qreal yLiaison = getPosAbsLiaison(voieSuivante).y();
    qreal x = posActuelle.x() - xLiaison;
    qreal y = posActuelle.y() - yLiaison;
    qreal distDel = sqrt((x*x) + (y*y));

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

void VoieCourbe::correctionPosition(qreal deltaX, qreal deltaY, Voie *v)
{
    //correction...
    if(ordreLiaison.key(v) ==0)
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

    qreal nouvelleCorde = sqrt(coordonneesLiaison[1]->x() *
                               coordonneesLiaison[1]->x() +
                               coordonneesLiaison[1]->y() *
                               coordonneesLiaison[1]->y());
    this->rayon = nouvelleCorde / (2.0 * sin((angle / 360.0) * PI));

    qreal anglePourCentre = atan2(- coordonneesLiaison[1]->y(), - coordonneesLiaison[1]->x()) -
                            direction * ((180.0 - angle) / 360.0) * PI;

    //calculer coordonnees du centre.
    centre.setX(- rayon * cos(anglePourCentre));
    centre.setY(- rayon * sin(anglePourCentre));

    setAngleRad(0, atan2(- centre.y(), centre.x()) + direction * PI / 2.0);

    setAngleDeg(1, getAngleDeg(0) + 180.0 + direction * angle);

    if(this->contact != nullptr)
        calculerPositionContact();
}


void VoieCourbe::correctionPositionLoco(qreal &/*x*/, qreal &/*y*/)
{
    //TODO
}


QRectF VoieCourbe::boundingRect() const
{
    return QRectF(QPointF(-200.0, -200.0),
                  QPointF(400.0, 400.0));
}

void VoieCourbe::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    painter->setPen(this->pen());
    painter->drawArc(QRectF(centre.x() - rayon, centre.y() - rayon, 2.0 * rayon, 2.0 * rayon),
                     static_cast<int>((getAngleDeg(0) - (direction * 270.0)) * 16.0),
                     static_cast<int>((direction * angle) *16.0));
    drawBoundingRect(painter);

/*    QPainterPath path;
    path.moveTo(0,0);
    path.cubicTo(0,80,80,160,160,160);
    painter->drawPath(path);
*/
}

void VoieCourbe::setEtat(int /*nouvelEtat*/)
{
    qDebug() << "Appel de setEtat sur une voie non variable.";
}
