#include "voieaiguillage.h"
#include "trainsimsettings.h"

/** Constructeur de classe VoieAiguillage.
  * @param ID, le numero d'identification de la voie.
  * @param rayon, le rayon de courbure de la partie courbe de l'aiguillage.
  * @param angle, l'angle parcouru par la partie courbe de l'aiguillage, exprime en degres. La conversion en radians se fait dans le constructeur.
  * @param longueur, la longueur de la partie droite de l'aiguillage.
  * @param direction, la direction de la partie courbe de l'aiguillage. (gauche = 1, droite = -1)
  */

VoieAiguillage::VoieAiguillage(qreal angle, qreal rayon, qreal longueur, qreal direction)
    :VoieVariable()
{
    setNewPen(COULEUR_AIGUILLAGE);
    this->rayon = rayon;
    this->angle = angle;
    this->longueur = longueur;
    this->direction = direction;
    this->etat = 0;
    this->orientee = false;
    this->posee = false;
    this->lastDistDel = 1000.0;
}
#include "ctrain_handler.h"

void VoieAiguillage::mousePressEvent ( QGraphicsSceneMouseEvent * /*event*/ )
{
    setEtat(1-this->etat);
#ifdef MAQUETTE

    diriger_aiguillage(this->numVoieVariable,this->etat,0);
#endif
    update();
//    Voie::mousePressEvent(event);
}

void VoieAiguillage::setNumVoieVariable(int numVoieVariable)
{
    this->numVoieVariable = numVoieVariable;
}

void VoieAiguillage::calculerAnglesEtCoordonnees(Voie *v)
{
    int ordreVoieFixe;
    if( v== nullptr)
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
        setAngleDeg(2, normaliserAngle(getAngleDeg(0) + 180.0 + direction * angle));
    }
    else if (ordreVoieFixe == 1)
    {
        setAngleDeg(0, normaliserAngle(getAngleDeg(1) + 180.0));
        setAngleDeg(2, normaliserAngle(getAngleDeg(0) + 180.0 + direction * angle));
    }
    else if(ordreVoieFixe == 2)
    {
        setAngleDeg(0, normaliserAngle(getAngleDeg(2) + 180.0 - direction * angle));
        setAngleDeg(1, normaliserAngle(getAngleDeg(0) + 180.0));
    }

    //calculer coordonnees du centre.
    centre.setX(rayon * cos(getAngleRad(0) - (direction / 2.0) * PI));
    centre.setY(- rayon * sin(getAngleRad(0) - (direction / 2.0) * PI));

    //calculer position relative de 0 et 1.
    coordonneesLiaison[0]->setX(0.0);
    coordonneesLiaison[0]->setY(0.0);
    coordonneesLiaison[1]->setX(longueur * cos(getAngleRad(1)));
    coordonneesLiaison[1]->setY(- longueur * sin(getAngleRad(1)));
    coordonneesLiaison[2]->setX(centre.x() + rayon * cos(getAngleRad(2) - (direction / 2.0) * PI));
    coordonneesLiaison[2]->setY(centre.y() - rayon * sin(getAngleRad(2) - (direction / 2.0) * PI));

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

void VoieAiguillage::calculerPositionContact()
{
    //dummy code. A priori, on ne met pas de contact sur un aiguillage.
    this->contact->setPos(0.0,0.0);
}

QList<QList<Voie*>*> VoieAiguillage::explorationContactAContact(Voie* voieAppelante)
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

qreal VoieAiguillage::getLongueurAParcourir()
{
    if(etat == TOUT_DROIT)
    {
        return longueur;
    }
    else
    {
        return 2.0 * (angle * PI / 180.0) * rayon;
    }
}

void VoieAiguillage::avanceLoco(qreal &dist, qreal &angle, qreal &rayon, qreal angleCumule, QPointF posActuelle, Voie *voieSuivante)
{
    if(ordreLiaison.key(voieSuivante) == 0)
    {
        if(normaliserAngle(angleCumule - getAngleDeg(1) - 180.0) < 1.0 &&
           normaliserAngle(angleCumule - getAngleDeg(1) - 180.0) > -1.0)
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
            rayon = this->rayon;

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
    }
    else
    {
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
            rayon = this->rayon;

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
    {
        lastDistDel = 1000.0;
    }
}

void VoieAiguillage::correctionPosition(qreal deltaX, qreal deltaY, Voie *v)
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
    this->rayon = nouvelleCorde / (2.0 * sin((angle / 360.0) * PI));

    qreal anglePourCentre = atan2(- coordonneesLiaison[2]->y(), - coordonneesLiaison[2]->x()) -
                            direction * ((180.0 - angle) / 360.0) * PI;

    //calculer coordonnees du centre.
    centre.setX(- rayon * cos(anglePourCentre));
    centre.setY(- rayon * sin(anglePourCentre));

    setAngleRad(0, atan2(- centre.y(), centre.x()) + direction * PI / 2.0);

    setAngleDeg(2, getAngleDeg(0) + 180.0 + direction * angle);


    if(this->contact != nullptr)
        calculerPositionContact();
}


void VoieAiguillage::correctionPositionLoco(qreal &/*x*/, qreal &/*y*/)
{
    //TODO
}


#define min(a,b) (a<b?a:b)
#define min3(a,b,c) (a<min(b,c)?a:min(b,c))

#define max(a,b) (a>b?a:b)
#define max3(a,b,c) (a>max(b,c)?a:max(b,c))

QRectF VoieAiguillage::boundingRect() const
{
    qreal x1=min3(coordonneesLiaison[0]->x(),coordonneesLiaison[1]->x(),coordonneesLiaison[2]->x());
    qreal x2=max3(coordonneesLiaison[0]->x(),coordonneesLiaison[1]->x(),coordonneesLiaison[2]->x());
    qreal y1=min3(coordonneesLiaison[0]->y(),coordonneesLiaison[1]->y(),coordonneesLiaison[2]->y());
    qreal y2=max3(coordonneesLiaison[0]->y(),coordonneesLiaison[1]->y(),coordonneesLiaison[2]->y());
    QRectF rect=QRectF(QPointF(x1,y1),QPointF(x2,y2));
    rect.adjust(-LARGEUR_VOIE,-LARGEUR_VOIE,LARGEUR_VOIE,LARGEUR_VOIE);
    return rect;
}

void VoieAiguillage::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    QPen p1=this->pen();
    QPen p2=this->pen();
    p2.setColor(Qt::gray);


    if (etat)
    {
        painter->setPen(p2);
        painter->drawArc(QRectF(centre.x() - rayon, centre.y() - rayon, 2.0 * rayon, 2.0 * rayon),
                         static_cast<int>((getAngleDeg(0) - (direction * 270.0)) *16),
                         static_cast<int>((direction * angle) *16));
        painter->setPen(p1);
        painter->drawLine(*coordonneesLiaison[0], *coordonneesLiaison[1]);
    }
    else
    {
        painter->setPen(p2);
        painter->drawLine(*coordonneesLiaison[0], *coordonneesLiaison[1]);
        painter->setPen(p1);
        painter->drawArc(QRectF(centre.x() - rayon, centre.y() - rayon, 2.0 * rayon, 2.0 * rayon),
                         static_cast<int>((getAngleDeg(0) - (direction * 270.0)) *16.0),
                         static_cast<int>((direction * angle) *16.0));
    }

    drawBoundingRect(painter);


    if (TrainSimSettings::getInstance()->getViewAiguillageNumber())
    {
        painter->setPen(Qt::red);

        painter->setFont(QFont("Verdana", 30, 99));

        QRectF rect;

        qreal angleTranslation = atan2(- coordonneesLiaison[1]->y(), - coordonneesLiaison[1]->x()) - PI / 2.0;

/*        if (angleTranslation>PI)
            angleTranslation-=PI;
        if (angleTranslation<0.0)
            angleTranslation+=PI;
  */
        if (direction==1.0)
            rect = QRectF((coordonneesLiaison[1]->x()-coordonneesLiaison[0]->x())/2+TRANSLATION_NUM_CONTACT * cos(angleTranslation) - 3.0 * TAILLE_CONTACT,
                        (coordonneesLiaison[1]->y()-coordonneesLiaison[0]->y())/2+TRANSLATION_NUM_CONTACT * sin(angleTranslation) - 3.0 * TAILLE_CONTACT,
                        6.0 * TAILLE_CONTACT,
                        6.0 * TAILLE_CONTACT); //meme constantes que pour les contacts.
        else
            rect = QRectF((coordonneesLiaison[1]->x()-coordonneesLiaison[0]->x())/2-TRANSLATION_NUM_CONTACT * cos(angleTranslation) - 3.0 * TAILLE_CONTACT,
                        (coordonneesLiaison[1]->y()-coordonneesLiaison[0]->y())/2-TRANSLATION_NUM_CONTACT * sin(angleTranslation) - 3.0 * TAILLE_CONTACT,
                        6.0 * TAILLE_CONTACT,
                        6.0 * TAILLE_CONTACT); //meme constantes que pour les contacts.

        QString t;
        t.setNum(numVoieVariable);

        painter->drawText(rect,Qt::AlignCenter, t);
     //   painter->drawLine(QPointF(0.0,0.0), QPointF(TRANSLATION_NUM_CONTACT * cos(angleTranslation) / 2.0, TRANSLATION_NUM_CONTACT * sin(angleTranslation) / 2.0));

    }
}

Voie* VoieAiguillage::getVoieSuivante(Voie *voieArrivee)
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
