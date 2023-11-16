#include "voieaiguillageenroule.h"
#include "trainsimsettings.h"

VoieAiguillageEnroule::VoieAiguillageEnroule(qreal angle, qreal rayon, qreal longueur, qreal direction)
    :VoieVariable()
{
    setNewPen(COULEUR_AIGUILLAGE);
    this->rayonInterieur = rayon;
    this->rayonExterieur = rayon;
    this->angle = angle;
    this->longueur = longueur;
    this->direction = direction;
    this->etat = 0;
    this->orientee = false;
    this->posee = false;
    this->lastDistDel = 1000.0;
}

void VoieAiguillageEnroule::mousePressEvent ( QGraphicsSceneMouseEvent * /*event*/ )
{
    setEtat(1-this->etat);
    update();
}

void VoieAiguillageEnroule::setNumVoieVariable(int numVoieVariable)
{
    this->numVoieVariable = numVoieVariable;
}

void VoieAiguillageEnroule::calculerAnglesEtCoordonnees(Voie *v)
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
        setAngleDeg(1, normaliserAngle(getAngleDeg(0) + 180.0 + direction * angle));
        setAngleDeg(2, normaliserAngle(getAngleDeg(0) + 180.0 + direction * angle));
    }
    else if (ordreVoieFixe == 1)
    {
        setAngleDeg(0, normaliserAngle(getAngleDeg(1) + 180.0 - direction * angle));
        setAngleDeg(2, normaliserAngle(getAngleDeg(1)));
    }
    else if(ordreVoieFixe == 2)
    {
        setAngleDeg(0, normaliserAngle(getAngleDeg(2) + 180.0 - direction * angle));
        setAngleDeg(1, normaliserAngle(getAngleDeg(2)));
    }

    //calculer coordonnees du centre.
    centreInterieur.setX(rayonInterieur * cos(getAngleRad(0) - (direction / 2.0) * PI));
    centreInterieur.setY(- rayonInterieur * sin(getAngleRad(0) - (direction / 2.0) * PI));
    centreExterieur.setX(longueur * cos(getAngleRad(0) + PI) +
                          rayonExterieur * cos(getAngleRad(0) - (direction / 2.0) * PI));
    centreExterieur.setY(- longueur * sin(getAngleRad(0) + PI) -
                          rayonExterieur * sin(getAngleRad(0) - (direction / 2.0) * PI));

    //calculer position relative de 0 et 1.
    coordonneesLiaison[0]->setX(0.0);
    coordonneesLiaison[0]->setY(0.0);
    coordonneesLiaison[1]->setX(centreExterieur.x() + rayonExterieur * cos(getAngleRad(2) - (direction / 2.0) * PI));
    coordonneesLiaison[1]->setY(centreExterieur.y() - rayonExterieur * sin(getAngleRad(2) - (direction / 2.0) * PI));
    coordonneesLiaison[2]->setX(centreInterieur.x() + rayonInterieur * cos(getAngleRad(1) - (direction / 2.0) * PI));
    coordonneesLiaison[2]->setY(centreInterieur.y() - rayonInterieur * sin(getAngleRad(1) - (direction / 2.0) * PI));

    if(this->contact != nullptr)
        calculerPositionContact();

    orientee = true;

    if(!ordreLiaison[0]->estOrientee())
        ordreLiaison[0]->calculerAnglesEtCoordonnees(this);
    if(!ordreLiaison[1]->estOrientee())
        ordreLiaison[1]->calculerAnglesEtCoordonnees(this);
    if(!ordreLiaison[2]->estOrientee())
        ordreLiaison[2]->calculerAnglesEtCoordonnees(this);
}


void VoieAiguillageEnroule::correctionPositionLoco(qreal &/*x*/, qreal &/*y*/)
{
    //TODO
}


void VoieAiguillageEnroule::calculerPositionContact()
{
    //dummy code. A priori, on ne met pas de contact sur un aiguillage.
    this->contact->setPos(0.0,0.0);
}

QList<QList<Voie*>*> VoieAiguillageEnroule::explorationContactAContact(Voie* voieAppelante)
{
    QList<QList<Voie*>*> temp;

    if(this->contact == nullptr)
    {
        if(ordreLiaison.key(voieAppelante) == 0)
        {
            temp.append(ordreLiaison[1]->explorationContactAContact(this));
            temp.append(ordreLiaison[2]->explorationContactAContact(this));
        }
        else
        {
            temp.append(ordreLiaison[0]->explorationContactAContact(this));
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

qreal VoieAiguillageEnroule::getLongueurAParcourir()
{
    if(etat == TOUT_DROIT)
    {
        return longueur + 2.0 * (angle * PI / 180.0) * rayonExterieur;
    }
    else
    {
        return 2.0 * (angle * PI / 180.0) * rayonInterieur;
    }
}

void VoieAiguillageEnroule::avanceLoco(qreal &dist, qreal &angle, qreal &rayon, qreal angleCumule, QPointF posActuelle, Voie *voieSuivante)
{
    QPointF positionLocoRelative = mapFromParent(posActuelle);

    if(ordreLiaison.key(voieSuivante) == 0)
    {
        if(sqrt((positionLocoRelative.x() - centreInterieur.x()) * (positionLocoRelative.x() - centreInterieur.x()) +
                (positionLocoRelative.y() - centreInterieur.y()) * (positionLocoRelative.y() - centreInterieur.y())) - this->rayonInterieur < 0.1 &&
           sqrt((positionLocoRelative.x() - centreInterieur.x()) * (positionLocoRelative.x() - centreInterieur.x()) +
                (positionLocoRelative.y() - centreInterieur.y()) * (positionLocoRelative.y() - centreInterieur.y())) - this->rayonInterieur > -0.1)
        {
            rayon = this->rayonInterieur;

            qreal angleAParcourir = (dist / rayon) * (180.0 / PI);

            qreal angleRestant = (getAngleDeg(0) + 360.0) - (angleCumule + 360.0);

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
        else
        {
            if(sqrt((positionLocoRelative.x()) * (positionLocoRelative.x()) +
                    (positionLocoRelative.y()) * (positionLocoRelative.y())) < longueur)
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
                rayon = this->rayonExterieur;

                qreal angleAParcourir = (dist / rayon) * (180.0 / PI);

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
            }
        }
    }
    else
    {
        if(etat == TOUT_DROIT)
        {
            if(sqrt((positionLocoRelative.x()) * (positionLocoRelative.x()) +
                    (positionLocoRelative.y()) * (positionLocoRelative.y())) < longueur)
            {
                angle = 0.0;
                rayon = 0.0;

                if(sqrt((posActuelle.x() - getPosAbsLiaison(voieSuivante).x()) *
                        (posActuelle.x() - getPosAbsLiaison(voieSuivante).x()) +
                        (posActuelle.y() - getPosAbsLiaison(voieSuivante).y()) *
                        (posActuelle.y() - getPosAbsLiaison(voieSuivante).y())) - longueur < dist)
                {
                    dist -= sqrt((posActuelle.x() - getPosAbsLiaison(voieSuivante).x()) *
                                 (posActuelle.x() - getPosAbsLiaison(voieSuivante).x()) +
                                 (posActuelle.y() - getPosAbsLiaison(voieSuivante).y()) *
                                 (posActuelle.y() - getPosAbsLiaison(voieSuivante).y())) - longueur;
                }
                else
                {
                    dist = 0.0;
                }
            }
            else
            {
                rayon = this->rayonExterieur;

                qreal angleAParcourir = (dist / rayon) * (180.0 / PI);

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
            }
        }
        else
        {
            rayon = this->rayonInterieur;

            qreal angleAParcourir = (dist / rayon) * (180.0 / PI);

            qreal angleRestant = (getAngleDeg(2) + 360.0) - (angleCumule + 360.0);

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

void VoieAiguillageEnroule::correctionPosition(qreal deltaX, qreal deltaY, Voie *v)
{
    //correction
    if(ordreLiaison.key(v) == 0)
    {
        setPos(this->pos().x() + deltaX, this->pos().y() + deltaY);
        coordonneesLiaison[1]->setX(coordonneesLiaison[1]->x() - deltaX);
        coordonneesLiaison[1]->setY(coordonneesLiaison[1]->y() - deltaY);
        coordonneesLiaison[2]->setX(coordonneesLiaison[2]->x() - deltaX);
        coordonneesLiaison[2]->setY(coordonneesLiaison[2]->y() - deltaY);
    }
    else
    {
        coordonneesLiaison[ordreLiaison.key(v)]->setX(coordonneesLiaison[ordreLiaison.key(v)]->x() + deltaX);
        coordonneesLiaison[ordreLiaison.key(v)]->setY(coordonneesLiaison[ordreLiaison.key(v)]->y() + deltaY);
    }

    qreal nouvelleCorde = sqrt(coordonneesLiaison[2]->x() *
                               coordonneesLiaison[2]->x() +
                               coordonneesLiaison[2]->y() *
                               coordonneesLiaison[2]->y());
    this->rayonInterieur = nouvelleCorde / (2.0 * sin((angle / 360.0) * PI));

    qreal anglePourCentre = atan2(- coordonneesLiaison[2]->y(), - coordonneesLiaison[2]->x()) -
                           direction * ((180.0 - angle) / 360.0) * PI;

    //calculer coordonnees du centre interieur.
    centreInterieur.setX(- rayonInterieur * cos(anglePourCentre));
    centreInterieur.setY(- rayonInterieur * sin(anglePourCentre));

    nouvelleCorde = sqrt((coordonneesLiaison[1]->x()- longueur * cos(getAngleRad(0) + PI)) *
                         (coordonneesLiaison[1]->x()- longueur * cos(getAngleRad(0) + PI)) +
                         (- coordonneesLiaison[1]->y()- longueur * sin(getAngleRad(0) + PI)) *
                         (- coordonneesLiaison[1]->y()- longueur * sin(getAngleRad(0) + PI)));
    this->rayonExterieur = nouvelleCorde / (2.0 * sin((angle / 360.0) * PI));

    anglePourCentre = atan2(+ coordonneesLiaison[1]->y() - longueur * sin(getAngleRad(0)),
                            - coordonneesLiaison[1]->x()- longueur * cos(getAngleRad(0))) -
                            direction * ((180.0 - angle) / 360.0) * PI;

    //calculer coordonnees du centre exterieur.
    centreExterieur.setX(coordonneesLiaison[1]->x() + rayonExterieur * cos(anglePourCentre));
    centreExterieur.setY(coordonneesLiaison[1]->y() - rayonExterieur * sin(anglePourCentre));


    setAngleRad(0, atan2(- centreInterieur.y(), centreInterieur.x()) + direction * PI / 2.0);

    setAngleRad(1, atan2(- centreExterieur.y() + coordonneesLiaison[1]->y(),
                         centreExterieur.x() - coordonneesLiaison[1]->x()) - direction * PI / 2.0);

    setAngleRad(2, atan2(- centreInterieur.y() - coordonneesLiaison[2]->y(),
                         centreInterieur.x() + coordonneesLiaison[2]->x()) - direction * PI / 2.0);

    if(this->contact != nullptr)
        calculerPositionContact();
}

#define min(a,b) (a<b?a:b)
#define min3(a,b,c) (a<min(b,c)?a:min(b,c))

#define max(a,b) (a>b?a:b)
#define max3(a,b,c) (a>max(b,c)?a:max(b,c))

QRectF VoieAiguillageEnroule::boundingRect() const
{
    qreal x1=min3(coordonneesLiaison[0]->x(),coordonneesLiaison[1]->x(),coordonneesLiaison[2]->x());
    qreal x2=max3(coordonneesLiaison[0]->x(),coordonneesLiaison[1]->x(),coordonneesLiaison[2]->x());
    qreal y1=min3(coordonneesLiaison[0]->y(),coordonneesLiaison[1]->y(),coordonneesLiaison[2]->y());
    qreal y2=max3(coordonneesLiaison[0]->y(),coordonneesLiaison[1]->y(),coordonneesLiaison[2]->y());
    QRectF rect=QRectF(QPointF(x1,y1),QPointF(x2,y2));
    rect.adjust(-LARGEUR_VOIE,-LARGEUR_VOIE,LARGEUR_VOIE,LARGEUR_VOIE);
    return rect;
}

void VoieAiguillageEnroule::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    QPen p1=this->pen();
    QPen p2=this->pen();
    p2.setColor(Qt::gray);


    if (etat)
    {
        painter->setPen(p2);
        painter->drawArc(QRectF(centreInterieur.x() - rayonInterieur,
                                centreInterieur.y() - rayonInterieur,
                                2.0 * rayonInterieur,
                                2.0 * rayonInterieur),
                     static_cast<int>((getAngleDeg(0) - (direction * 270.0)) *16),
                     static_cast<int>((direction * angle) *16));
        painter->setPen(p1);
        painter->drawLine(*coordonneesLiaison[0], QPointF(longueur * cos(getAngleRad(0) + PI),
                                                          - longueur * sin(getAngleRad(0) + PI)));
        painter->drawArc(QRectF(centreExterieur.x() - rayonExterieur,
                                centreExterieur.y() - rayonExterieur,
                                2.0 * rayonExterieur,
                                2.0 * rayonExterieur),
                     static_cast<int>((getAngleDeg(0) - (direction * 270.0)) *16),
                     static_cast<int>((direction * angle) *16));
    }
    else
    {
        painter->setPen(p2);
        painter->drawLine(*coordonneesLiaison[0], QPointF(longueur * cos(getAngleRad(0) + PI),
                                                          - longueur * sin(getAngleRad(0) + PI)));
        painter->drawArc(QRectF(centreExterieur.x() - rayonExterieur,
                                centreExterieur.y() - rayonExterieur,
                                2.0 * rayonExterieur,
                                2.0 * rayonExterieur),
                     static_cast<int>((getAngleDeg(0) - (direction * 270.0)) *16),
                     static_cast<int>((direction * angle) *16));
        painter->setPen(p1);
        painter->drawArc(QRectF(centreInterieur.x() - rayonInterieur,
                                centreInterieur.y() - rayonInterieur,
                                2.0 * rayonInterieur,
                                2.0 * rayonInterieur),
                     static_cast<int>((getAngleDeg(0) - (direction * 270.0)) *16),
                     static_cast<int>((direction * angle) *16));
    }

    drawBoundingRect(painter);

    if (TrainSimSettings::getInstance()->getViewAiguillageNumber())
    {
        painter->setPen(Qt::red);

        painter->setFont(QFont("Verdana", 30, 99));

        QRectF rect;

        qreal angleTranslation = atan2(- coordonneesLiaison[1]->y(), - coordonneesLiaison[1]->x()) - PI / 2.0;

        rect = QRectF(TRANSLATION_NUM_CONTACT * cos(angleTranslation) - 3.0 * TAILLE_CONTACT,
                      TRANSLATION_NUM_CONTACT * sin(angleTranslation) - 3.0 * TAILLE_CONTACT,
                      6.0 * TAILLE_CONTACT,
                      6.0 * TAILLE_CONTACT); //meme constantes que pour les contacts.

        QString t;
        t.setNum(numVoieVariable);

        painter->drawText(rect, t);
        painter->drawLine(QPointF(0.0,0.0), QPointF(TRANSLATION_NUM_CONTACT * cos(angleTranslation) / 2.0, TRANSLATION_NUM_CONTACT * sin(angleTranslation) / 2.0));

    }
}

Voie* VoieAiguillageEnroule::getVoieSuivante(Voie *voieArrivee)
{
    //gestion des deraillements!

    int ordreVoieArrivee = ordreLiaison.key(voieArrivee);

    if(ordreVoieArrivee == 0)
    {
        if(etat == TOUT_DROIT)
        {
            return ordreLiaison.value(1);
        }
        else
        {
            return ordreLiaison.value(2);
        }
    }
    else return ordreLiaison.value(0);
}
