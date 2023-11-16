#include "voietraverseejonction.h"

VoieTraverseeJonction::VoieTraverseeJonction(qreal angle, qreal rayon, qreal longueur)
    :VoieVariable()
{
    setNewPen(COULEUR_TRAVERSEEJONCTION);
    this->rayon03 = this->rayon12 = rayon;
    this->angle = angle;
    this->longueur = longueur;
    this->etat = 0;
    this->orientee = false;
    this->posee = false;
    this->lastDistDel = 1000.0;
}

void VoieTraverseeJonction::setNumVoieVariable(int numVoieVariable)
{
    this->numVoieVariable = numVoieVariable;
}

void VoieTraverseeJonction::calculerAnglesEtCoordonnees(Voie *v)
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
    coordonneesLiaison[2]->setY(- ((longueur / 2.0) * (sin(getAngleRad(1)) + sin(getAngleRad(2)))));
    coordonneesLiaison[3]->setX((longueur / 2.0) * (cos(getAngleRad(1)) + cos(getAngleRad(3))));
    coordonneesLiaison[3]->setY(- ((longueur / 2.0) * (sin(getAngleRad(1)) + sin(getAngleRad(3)))));

    //calculer coordonnees du centre12.
    centre12.setX(coordonneesLiaison.value(1)->x() + rayon12 * cos(getAngleRad(1) - PI / 2.0));
    centre12.setY(coordonneesLiaison.value(1)->y() +- rayon12 * sin(getAngleRad(1) - PI / 2.0));
    //calculer coordonnees du centre03.
    centre03.setX(rayon03 * cos(getAngleRad(0) - PI / 2.0));
    centre03.setY(- rayon03 * sin(getAngleRad(0) - PI / 2.0));


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

void VoieTraverseeJonction::calculerPositionContact()
{
    //dummy code. A priori, on ne met pas de contact dans une traversee-jonction.
    this->contact->setPos(0.0,0.0);
}

QList<QList<Voie*>*> VoieTraverseeJonction::explorationContactAContact(Voie* voieAppelante)
{
    QList<QList<Voie*>*> temp;

    if(this->contact == nullptr)
    {
        if(ordreLiaison.key(voieAppelante) == 0 || ordreLiaison.key(voieAppelante) == 2)
        {
            temp.append(ordreLiaison[1]->explorationContactAContact(this));
            temp.append(ordreLiaison[3]->explorationContactAContact(this));
        }
        else if(ordreLiaison.key(voieAppelante) == 1 || ordreLiaison.key(voieAppelante) == 3)
        {
            temp.append(ordreLiaison[0]->explorationContactAContact(this));
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

qreal VoieTraverseeJonction::getLongueurAParcourir()
{
    if(etat == TOUT_DROIT)
    {
        return longueur;
    }
    else
    {
        return (angle * PI / 180.0) * (rayon12 + rayon03); //approximation suffisante.
    }
}

Voie* VoieTraverseeJonction::getVoieSuivante(Voie *voieArrivee)
{
    int ordreVoieArrivee = ordreLiaison.key(voieArrivee);

    if(this->etat == TOUT_DROIT)
    {
        if( ordreVoieArrivee == 0)
            return ordreLiaison.value(1);
        else if (ordreVoieArrivee == 1)
            return ordreLiaison.value(0);
        else if (ordreVoieArrivee == 2)
            return ordreLiaison.value(3);
        else
            return ordreLiaison.value(2);
    }
    else
    {
        if( ordreVoieArrivee == 0)
            return ordreLiaison.value(3);
        else if (ordreVoieArrivee == 1)
            return ordreLiaison.value(2);
        else if (ordreVoieArrivee == 2)
            return ordreLiaison.value(1);
        else
            return ordreLiaison.value(0);
    }
}

void VoieTraverseeJonction::avanceLoco(qreal &dist, qreal &angle, qreal &rayon, qreal angleCumule, QPointF posActuelle, Voie *voieSuivante)
{
    QPointF positionLocoRelative = mapFromParent(posActuelle);

    if(etat == TOUT_DROIT)
    {
        angle = 0.0;
        rayon = 0.0;

        if(sqrt((posActuelle.x() - getPosAbsLiaison(voieSuivante).x()) *
                (posActuelle.x() - getPosAbsLiaison(voieSuivante).x()) +
                (posActuelle.y() - getPosAbsLiaison(voieSuivante).y()) *
                (posActuelle.y() - getPosAbsLiaison(voieSuivante).y())) < dist)
        {
            dist -= sqrt((posActuelle.x() - getPosAbsLiaison(voieSuivante).x()) *
                         (posActuelle.x() - getPosAbsLiaison(voieSuivante).x()) +
                         (posActuelle.y() - getPosAbsLiaison(voieSuivante).y()) *
                         (posActuelle.y() - getPosAbsLiaison(voieSuivante).y()));
        }
        else
        {
            dist = 0.0;
        }
    }
    else
    {
        if(sqrt((positionLocoRelative.x() - centre03.x()) *
                (positionLocoRelative.x() - centre03.x()) +
                (positionLocoRelative.y() - centre03.y()) *
                (positionLocoRelative.y() - centre03.y())) - rayon03 < 0.1 &&
           sqrt((positionLocoRelative.x() - centre03.x()) *
                (positionLocoRelative.x() - centre03.x()) +
                (positionLocoRelative.y() - centre03.y()) *
                (positionLocoRelative.y() - centre03.y())) - rayon03 > -0.1)
        {
            rayon = this->rayon03;
        }
        else
        {
            rayon = this->rayon12;
        }
        qreal angleAParcourir = (dist / rayon) * (180.0 / PI);

        qreal angleRestant = (getAngleDeg(ordreLiaison.key(voieSuivante)) + 360.0) - (angleCumule + 360.0);

        while(angleRestant < - this->angle)
        {
            angleRestant += 360.0;
        }
        if(angleRestant > this->angle)
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

void VoieTraverseeJonction::correctionPosition(qreal deltaX, qreal deltaY, Voie *v)
{
    //Correction
    if(ordreLiaison.key(v) == 0)
    {
        setPos(this->pos().x() + deltaX, this->pos().y() + deltaY);
        coordonneesLiaison[1]->setX(coordonneesLiaison[1]->x() - deltaX);
        coordonneesLiaison[1]->setY(coordonneesLiaison[1]->y() - deltaY);
        coordonneesLiaison[2]->setX(coordonneesLiaison[2]->x() - deltaX);
        coordonneesLiaison[2]->setY(coordonneesLiaison[2]->y() - deltaY);
        coordonneesLiaison[3]->setX(coordonneesLiaison[2]->x() - deltaX);
        coordonneesLiaison[3]->setY(coordonneesLiaison[2]->y() - deltaY);
    }
    else
    {
        coordonneesLiaison[ordreLiaison.key(v)]->setX(coordonneesLiaison[ordreLiaison.key(v)]->x() + deltaX);
        coordonneesLiaison[ordreLiaison.key(v)]->setY(coordonneesLiaison[ordreLiaison.key(v)]->y() + deltaY);
    }

    //modifications pour courbe 03.
    qreal nouvelleCorde = sqrt(coordonneesLiaison[3]->x() *
                               coordonneesLiaison[3]->x() +
                               coordonneesLiaison[3]->y() *
                               coordonneesLiaison[3]->y());
    this->rayon03 = nouvelleCorde / (2.0 * sin((angle / 360.0) * PI));

    qreal anglePourCentre = atan2(- coordonneesLiaison[3]->y(), - coordonneesLiaison[3]->x()) -
                            ((180.0 - angle) / 360.0) * PI;

    //calculer coordonnees du centre 03.
    centre03.setX(- rayon03 * cos(anglePourCentre));
    centre03.setY(- rayon03 * sin(anglePourCentre));

    //modifications pour courbe 12.
    nouvelleCorde = sqrt((coordonneesLiaison[1]->x() - coordonneesLiaison[2]->x()) *
                         (coordonneesLiaison[1]->x() - coordonneesLiaison[2]->x()) +
                         (coordonneesLiaison[1]->y() - coordonneesLiaison[2]->y()) *
                         (coordonneesLiaison[1]->y() - coordonneesLiaison[2]->y()));
    this->rayon12 = nouvelleCorde / (2.0 * sin((angle / 360.0) * PI));

    anglePourCentre = atan2(- coordonneesLiaison[1]->y() + coordonneesLiaison[2]->y(), - coordonneesLiaison[1]->x() + coordonneesLiaison[2]->x()) +
                      ((180.0 - angle) / 360.0) * PI;


    //calculer coordonnees du centre 12.
    centre12.setX(coordonneesLiaison[2]->x() - rayon12 * cos(anglePourCentre));
    centre12.setY(coordonneesLiaison[2]->y() - rayon12 * sin(anglePourCentre));

    setAngleRad(0, atan2(- centre03.y(), centre03.x()) + PI / 2.0);

    setAngleRad(1, atan2(- centre12.y() + coordonneesLiaison[1]->y(),
                         centre12.x() - coordonneesLiaison[1]->x()) + PI / 2.0);

    setAngleRad(2, atan2(- centre12.y() + coordonneesLiaison[2]->y(),
                         centre12.x() - coordonneesLiaison[2]->x()) - PI / 2.0);

    setAngleRad(3, atan2(- centre03.y() + coordonneesLiaison[3]->y(),
                         centre03.x() - coordonneesLiaison[3]->x()) - PI / 2.0);

    if(this->contact != nullptr)
        calculerPositionContact();

}


void VoieTraverseeJonction::correctionPositionLoco(qreal &/*x*/, qreal &/*y*/)
{
    //TODO
}


QRectF VoieTraverseeJonction::boundingRect() const
{
    return QRectF(QPointF(-200.0, -200.0),
                  QPointF(400.0, 400.0));
}

void VoieTraverseeJonction::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    QPen p1=this->pen();
    QPen p2=this->pen();
    p2.setColor(Qt::gray);


    if (etat)
    {
        painter->setPen(p2);
        painter->drawArc(QRectF(centre12.x() - rayon12, centre12.y() - rayon12, 2.0 * rayon12, 2.0 * rayon12),
                         static_cast<int>((getAngleDeg(1) - 270.0) *16),
                         static_cast<int>(angle *16));
        painter->drawArc(QRectF(centre03.x() - rayon03, centre03.y() - rayon03, 2.0 * rayon03, 2.0 * rayon03),
                         static_cast<int>((getAngleDeg(3) + 270.0) *16),
                         static_cast<int>(- angle *16));
        painter->setPen(p1);
        painter->drawLine(*coordonneesLiaison[0], *coordonneesLiaison[1]);
        painter->drawLine(*coordonneesLiaison[2], *coordonneesLiaison[3]);
    }
    else
    {
        painter->setPen(p2);
        painter->drawLine(*coordonneesLiaison[0], *coordonneesLiaison[1]);
        painter->drawLine(*coordonneesLiaison[2], *coordonneesLiaison[3]);
        painter->setPen(p1);
        painter->drawArc(QRectF(centre12.x() - rayon12, centre12.y() - rayon12, 2.0 * rayon12, 2.0 * rayon12),
                         static_cast<int>((getAngleDeg(1) - 270.0) *16),
                         static_cast<int>(angle *16));
        painter->drawArc(QRectF(centre03.x() - rayon03, centre03.y() - rayon03, 2.0 * rayon03, 2.0 * rayon03),
                         static_cast<int>((getAngleDeg(3) + 270.0) *16),
                         static_cast<int>(- angle *16));
    }


#ifdef DRAW_BOUNDINGRECT
    painter->setPen(Qt::DashLine);
    painter->drawRect(this->boundingRect());
#endif


    drawBoundingRect(painter);


    if (TrainSimSettings::getInstance()->getViewAiguillageNumber())
    {
        painter->setPen(Qt::red);

        painter->setFont(QFont("Verdana", 30, 99));

        QRectF rect;

        qreal angleTranslation = atan2(- coordonneesLiaison[1]->y(), - coordonneesLiaison[1]->x()) - PI / 2.0;

        rect = QRectF((coordonneesLiaison[1]->x()-coordonneesLiaison[0]->x())/2+TRANSLATION_NUM_CONTACT * cos(angleTranslation) - 3.0 * TAILLE_CONTACT,
                      (coordonneesLiaison[1]->y()-coordonneesLiaison[0]->y())/2+TRANSLATION_NUM_CONTACT * sin(angleTranslation) - 3.0 * TAILLE_CONTACT,
                      6.0 * TAILLE_CONTACT,
                      6.0 * TAILLE_CONTACT); //meme constantes que pour les contacts.

        QString t;
        t.setNum(numVoieVariable);

        painter->drawText(rect, Qt::AlignCenter, t);
        painter->drawLine(QPointF(0.0,0.0), QPointF(TRANSLATION_NUM_CONTACT * cos(angleTranslation) / 2.0, TRANSLATION_NUM_CONTACT * sin(angleTranslation) / 2.0));

    }
}

void VoieTraverseeJonction::mousePressEvent ( QGraphicsSceneMouseEvent * /*event*/ )
{
    setEtat(1-this->etat);
    update();
}
