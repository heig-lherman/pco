#include "voieaiguillagetriple.h"

#include "trainsimsettings.h"

/** Constructeur de classe VoieAiguillageTriple.
  * @param ID, le numero d'identification de la voie.
  * @param rayon, le rayon de courbure de la partie courbe de l'aiguillage.
  * @param angle, l'angle parcouru par la partie courbe de l'aiguillage, exprime en degres. La conversion en radians se fait dans le constructeur.
  * @param longueur, la longueur de la partie droite de l'aiguillage.
  */

VoieAiguillageTriple::VoieAiguillageTriple(qreal angle, qreal rayon, qreal longueur)
    :VoieVariable()
{
    setNewPen(COULEUR_AIGUILLAGE);
    this->rayonGauche = this->rayonDroite = rayon;
    this->angle = angle;
    this->longueur = longueur;
    this->etat = 0;
    this->orientee = false;
    this->posee = false;
    this->lastDistDel = 1000.0;
}

void VoieAiguillageTriple::mousePressEvent ( QGraphicsSceneMouseEvent * /*event*/ )
{
    setEtat(1 + this->etat);
    if(this->etat > 1)
        setEtat(-1);
    update();
}

void VoieAiguillageTriple::setNumVoieVariable(int numVoieVariable)
{
    this->numVoieVariable = numVoieVariable;
}

void VoieAiguillageTriple::calculerAnglesEtCoordonnees(Voie *v)
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
        setAngleDeg(2, normaliserAngle(getAngleDeg(0) + 180.0 + angle));
        setAngleDeg(3, normaliserAngle(getAngleDeg(0) + 180.0 - angle));
    }
    else if (ordreVoieFixe == 1)
    {
        setAngleDeg(0, normaliserAngle(getAngleDeg(1) + 180.0));
        setAngleDeg(2, normaliserAngle(getAngleDeg(0) + 180.0 + angle));
        setAngleDeg(3, normaliserAngle(getAngleDeg(0) + 180.0 - angle));
    }
    else if(ordreVoieFixe == 2)
    {
        setAngleDeg(0, normaliserAngle(getAngleDeg(2) + 180.0 - angle));
        setAngleDeg(1, normaliserAngle(getAngleDeg(0) + 180.0));
        setAngleDeg(3, normaliserAngle(getAngleDeg(0) + 180.0 - angle));
    }
    else if(ordreVoieFixe == 3)
    {
        setAngleDeg(0, normaliserAngle(getAngleDeg(3) + 180.0 + angle));
        setAngleDeg(1, normaliserAngle(getAngleDeg(0) + 180.0));
        setAngleDeg(2, normaliserAngle(getAngleDeg(0) + 180.0 + angle));
    }

    //calculer coordonnees des centres.
    centreGauche.setX(rayonGauche * cos(getAngleRad(0) - (PI / 2.0)));
    centreGauche.setY(- rayonGauche * sin(getAngleRad(0) - (PI / 2.0)));
    centreDroite.setX(rayonDroite * cos(getAngleRad(0) + (PI / 2.0)));
    centreDroite.setY(- rayonDroite * sin(getAngleRad(0) + (PI / 2.0)));

    //calculer position relative de 0 et 1.
    coordonneesLiaison[0]->setX(0.0);
    coordonneesLiaison[0]->setY(0.0);
    coordonneesLiaison[1]->setX(longueur * cos(getAngleRad(1)));
    coordonneesLiaison[1]->setY(- longueur * sin(getAngleRad(1)));
    coordonneesLiaison[2]->setX(centreGauche.x() + rayonGauche * cos(getAngleRad(2) - (PI / 2.0)));
    coordonneesLiaison[2]->setY(centreGauche.y() - rayonGauche * sin(getAngleRad(2) - (PI / 2.0)));
    coordonneesLiaison[3]->setX(centreDroite.x() + rayonDroite * cos(getAngleRad(3) + (PI / 2.0)));
    coordonneesLiaison[3]->setY(centreDroite.y() - rayonDroite * sin(getAngleRad(3) + (PI / 2.0)));

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

void VoieAiguillageTriple::calculerPositionContact()
{
    //dummy code. A priori, on ne met pas de contact sur un aiguillage.
    this->contact->setPos(0.0,0.0);
}

QList<QList<Voie*>*> VoieAiguillageTriple::explorationContactAContact(Voie* voieAppelante)
{
    QList<QList<Voie*>*> temp;

    if(this->contact == nullptr)
    {
        if(ordreLiaison.key(voieAppelante) == 0)
        {
            temp.append(ordreLiaison[1]->explorationContactAContact(this));
            temp.append(ordreLiaison[2]->explorationContactAContact(this));
            temp.append(ordreLiaison[3]->explorationContactAContact(this));
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

qreal VoieAiguillageTriple::getLongueurAParcourir()
{
    if(etat == TOUT_DROIT)
    {
        return longueur;
    }
    else if(etat == -1)
    {
        return 2.0 * (angle * PI / 180.0) * rayonDroite;
    }
    else
    {
        return 2.0 * (angle * PI / 180.0) * rayonGauche;
    }
}

void VoieAiguillageTriple::avanceLoco(qreal &dist, qreal &angle, qreal &rayon, qreal angleCumule, QPointF posActuelle, Voie *voieSuivante)
{
    QPointF positionLocoRelative = mapFromParent(posActuelle);

    if(ordreLiaison.key(voieSuivante) == 0)
    {
        if(angleCumule == normaliserAngle(getAngleDeg(1) - 180.0))
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
            if(sqrt((positionLocoRelative.x() - centreGauche.x()) *
                    (positionLocoRelative.x() - centreGauche.x()) +
                    (positionLocoRelative.y() - centreGauche.y()) *
                    (positionLocoRelative.y() - centreGauche.y())) - rayonGauche < 0.1 &&
               sqrt((positionLocoRelative.x() - centreGauche.x()) *
                    (positionLocoRelative.x() - centreGauche.x()) +
                    (positionLocoRelative.y() - centreGauche.y()) *
                    (positionLocoRelative.y() - centreGauche.y())) - rayonGauche > -0.1)
            {
                rayon = this->rayonGauche;
            }
            else
            {
                rayon = this->rayonDroite;
            }

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
            if(sqrt((positionLocoRelative.x() - centreGauche.x()) *
                    (positionLocoRelative.x() - centreGauche.x()) +
                    (positionLocoRelative.y() - centreGauche.y()) *
                    (positionLocoRelative.y() - centreGauche.y())) - rayonGauche < 0.1 &&
               sqrt((positionLocoRelative.x() - centreGauche.x()) *
                    (positionLocoRelative.x() - centreGauche.x()) +
                    (positionLocoRelative.y() - centreGauche.y()) *
                    (positionLocoRelative.y() - centreGauche.y())) - rayonGauche > -0.1)
            {
                rayon = this->rayonGauche;
            }
            else
            {
                rayon = this->rayonDroite;
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


void VoieAiguillageTriple::correctionPositionLoco(qreal &/*x*/, qreal &/*y*/)
{
    //TODO
}


void VoieAiguillageTriple::correctionPosition(qreal deltaX, qreal deltaY, Voie *v)
{
    //correction
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

    //modifications pour courbe gauche.
    qreal nouvelleCorde = sqrt(coordonneesLiaison[2]->x() *
                               coordonneesLiaison[2]->x() +
                               coordonneesLiaison[2]->y() *
                               coordonneesLiaison[2]->y());
    this->rayonGauche = nouvelleCorde / (2.0 * sin((angle / 360.0) * PI));

    qreal anglePourCentre = atan2(- coordonneesLiaison[2]->y(), - coordonneesLiaison[2]->x()) -
                            ((180.0 - angle) / 360.0) * PI;

    //calculer coordonnees du centre gauche.
    centreGauche.setX(- rayonGauche * cos(anglePourCentre));
    centreGauche.setY(- rayonGauche * sin(anglePourCentre));

    //modifications pour courbe droite.
    nouvelleCorde = sqrt(coordonneesLiaison[3]->x() *
                         coordonneesLiaison[3]->x() +
                         coordonneesLiaison[3]->y() *
                         coordonneesLiaison[3]->y());
    this->rayonDroite = nouvelleCorde / (2.0 * sin((angle / 360.0) * PI));

    anglePourCentre = atan2(- coordonneesLiaison[3]->y(), - coordonneesLiaison[3]->x()) +
                      ((180.0 - angle) / 360.0) * PI;


    //calculer coordonees du centre droit.
    centreDroite.setX(- rayonDroite * cos(anglePourCentre));
    centreDroite.setY(- rayonDroite * sin(anglePourCentre));

    setAngleRad(0, atan2(- centreGauche.y(), centreGauche.x()) + PI / 2.0);

    setAngleDeg(1, atan2(- coordonneesLiaison[1]->y(), -coordonneesLiaison[1]->x()));

    setAngleRad(2, atan2(- centreGauche.y() + coordonneesLiaison[2]->y(),
                         centreGauche.x() - coordonneesLiaison[2]->x()) - PI / 2.0);

    setAngleRad(3, atan2(- centreDroite.y() + coordonneesLiaison[3]->y(),
                         centreDroite.x() - coordonneesLiaison[3]->x()) + PI / 2.0);


    if(this->contact != nullptr)
        calculerPositionContact();
}

#define min(a,b) (a<b?a:b)
#define min3(a,b,c) (a<min(b,c)?a:min(b,c))
#define min4(a,b,c,d) (a<min3(b,c,d)?a:min3(b,c,d))

#define max(a,b) (a>b?a:b)
#define max3(a,b,c) (a>max(b,c)?a:max(b,c))
#define max4(a,b,c,d) (a>max3(b,c,d)?a:max3(b,c,d))


QRectF VoieAiguillageTriple::boundingRect() const
{
    qreal x1=min4(coordonneesLiaison[0]->x(),coordonneesLiaison[1]->x(),coordonneesLiaison[2]->x(),coordonneesLiaison[3]->x());
    qreal x2=max4(coordonneesLiaison[0]->x(),coordonneesLiaison[1]->x(),coordonneesLiaison[2]->x(),coordonneesLiaison[3]->x());
    qreal y1=min4(coordonneesLiaison[0]->y(),coordonneesLiaison[1]->y(),coordonneesLiaison[2]->y(),coordonneesLiaison[3]->y());
    qreal y2=max4(coordonneesLiaison[0]->y(),coordonneesLiaison[1]->y(),coordonneesLiaison[2]->y(),coordonneesLiaison[3]->y());
    QRectF rect=QRectF(QPointF(x1,y1),QPointF(x2,y2));
    rect.adjust(-LARGEUR_VOIE,-LARGEUR_VOIE,LARGEUR_VOIE,LARGEUR_VOIE);
    return rect;
}

void VoieAiguillageTriple::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    QPen p1=this->pen();
    QPen p2=this->pen();
    p2.setColor(Qt::gray);


    if (etat == 0)
    {
        painter->setPen(p2);
        painter->drawArc(QRectF(centreGauche.x() - rayonGauche, centreGauche.y() - rayonGauche, 2.0 * rayonGauche, 2.0 * rayonGauche),
                     static_cast<int>((getAngleDeg(0) *16.0 - 4320.0)),
                     static_cast<int>(angle *16.0));
        painter->drawArc(QRectF(centreDroite.x() - rayonDroite, centreDroite.y() - rayonDroite, 2.0 * rayonDroite, 2.0 * rayonDroite),
                     static_cast<int>(-(getAngleDeg(0) *16.0 - 4320.0)),
                     static_cast<int>(-(angle *16.0)));
        painter->setPen(p1);
        painter->drawLine(*coordonneesLiaison[0], *coordonneesLiaison[1]);
    }
    else if(etat == -1)
    {
        painter->setPen(p2);
        painter->drawLine(*coordonneesLiaison[0], *coordonneesLiaison[1]);
        painter->drawArc(QRectF(centreDroite.x() - rayonDroite, centreDroite.y() - rayonDroite, 2.0 * rayonDroite, 2.0 * rayonDroite),
                     static_cast<int>(-(getAngleDeg(0) *16.0 - 4320.0)),
                     static_cast<int>(-(angle *16.0)));
        painter->setPen(p1);
        painter->drawArc(QRectF(centreGauche.x() - rayonGauche, centreGauche.y() - rayonGauche, 2.0 * rayonGauche, 2.0 * rayonGauche),
                     static_cast<int>((getAngleDeg(0) *16.0 - 4320.0)),
                     static_cast<int>(angle *16.0));
    }
    else
    {
        painter->setPen(p2);
        painter->drawLine(*coordonneesLiaison[0], *coordonneesLiaison[1]);
        painter->drawArc(QRectF(centreGauche.x() - rayonGauche, centreGauche.y() - rayonGauche, 2.0 * rayonGauche, 2.0 * rayonGauche),
                     static_cast<int>((getAngleDeg(0) *16.0 - 4320.0)),
                     static_cast<int>(angle *16.0));
        painter->setPen(p1);
        painter->drawArc(QRectF(centreDroite.x() - rayonDroite, centreDroite.y() - rayonDroite, 2.0 * rayonDroite, 2.0 * rayonDroite),
                     static_cast<int>(-(getAngleDeg(0) *16.0 - 4320.0)),
                     static_cast<int>(-(angle *16.0)));
    }

    drawBoundingRect(painter);


    if (TrainSimSettings::getInstance()->getViewAiguillageNumber())
    {
        painter->setPen(Qt::red);

        painter->setFont(QFont("Verdana", 50, 99));

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

Voie* VoieAiguillageTriple::getVoieSuivante(Voie *voieArrivee)
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
