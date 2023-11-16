#include "voie.h"


/** Constructeur de classe Voie.
  *
  */
Voie::Voie()
{
    this->contact = nullptr;
    setZValue(ZVAL_VOIE);
}



void Voie::setNewPen(const QColor &color)
{
    QPen pen;
    pen.setColor(color);
    pen.setWidth(LARGEUR_VOIE);
    pen.setCapStyle(Qt::FlatCap);
    setPen(pen);
}

void Voie::calculerPosition(Voie *v)
{
    if(v == nullptr)
    {
        setPos(0.0, 0.0);
    }
    else
    {
        QPointF positionLiaison = v->getPosAbsLiaison(this);

        setPos(positionLiaison.x() - coordonneesLiaison[ordreLiaison.key(v)]->x(),
               positionLiaison.y() - coordonneesLiaison[ordreLiaison.key(v)]->y());
    }

    posee = true;

    qreal deltaX, deltaY;

    for(int i =0; i < ordreLiaison.size(); i++)
    {
        if(!ordreLiaison[i]->estPosee())
            ordreLiaison[i]->calculerPosition(this);
        else
        {
            if((getPosAbsLiaison(ordreLiaison[i]).x() - ordreLiaison[i]->getPosAbsLiaison(this).x()) < -1e-10 ||
               (getPosAbsLiaison(ordreLiaison[i]).y() - ordreLiaison[i]->getPosAbsLiaison(this).y()) < -1e-10 ||
               (getPosAbsLiaison(ordreLiaison[i]).x() - ordreLiaison[i]->getPosAbsLiaison(this).x()) > 1e-10 ||
               (getPosAbsLiaison(ordreLiaison[i]).y() - ordreLiaison[i]->getPosAbsLiaison(this).y()) > 1e-10)
            {
                deltaX = getPosAbsLiaison(ordreLiaison[i]).x() - ordreLiaison[i]->getPosAbsLiaison(this).x();
                deltaY = getPosAbsLiaison(ordreLiaison[i]).y() - ordreLiaison[i]->getPosAbsLiaison(this).y();


                ordreLiaison[i]->correctionPosition(deltaX / 2.0, deltaY / 2.0, this);
                this->correctionPosition(- deltaX / 2.0, - deltaY / 2.0, ordreLiaison[i]);

                deltaX = getPosAbsLiaison(ordreLiaison[i]).x() - ordreLiaison[i]->getPosAbsLiaison(this).x();
                deltaY = getPosAbsLiaison(ordreLiaison[i]).y() - ordreLiaison[i]->getPosAbsLiaison(this).y();
            }
        }
    }
}


QList<QList<Voie*>*> Voie::startExplorationContactAContact()
{
    QList<QList<Voie*>*> temp;

    for(int i=0; i<ordreLiaison.size(); i++)
    {
        temp.append(ordreLiaison[i]->explorationContactAContact(this));
    }

    foreach(QList<Voie*>* lv, temp)
    {
        lv->prepend(this);
    }

    return temp;
}

void Voie::lier(Voie *v, int ordre)
{
    ordreLiaison.insert(ordre, v);
    coordonneesLiaison.insert(ordre, new QPointF());
    angleLiaison.insert(ordre, 0.0);
}

bool Voie::estOrientee()
{
    return orientee;
}

bool Voie::estPosee()
{
    return posee;
}

double Voie::normaliserAngle(double angle) const
{
    while (angle < 0.0)
        angle += 360.0;

    while (angle > 360.0)
        angle -= 360.0;

    return angle;
}

QPointF Voie::getPosAbsLiaison(Voie *v)
{
    return QPointF(this->scenePos().x() + coordonneesLiaison[ordreLiaison.key(v)]->x(),
                                this->scenePos().y() + coordonneesLiaison[ordreLiaison.key(v)]->y());
}

void Voie::setContact(Contact *c)
{
    this->contact = c;
    c->setParentItem(this);
}

Contact* Voie::getContact() const
{
    return this->contact;
}

int Voie::getNbreLiaisons() const
{
    return ordreLiaison.size();
}

qreal Voie::getXmin() const
{
    return scenePos().x() + boundingRect().left();
}

qreal Voie::getXmax() const
{
    return scenePos().x() + boundingRect().right();
}

qreal Voie::getYmin() const
{
    return scenePos().y() + boundingRect().top();
}

qreal Voie::getYmax() const
{
    return scenePos().y() + boundingRect().bottom();
}

qreal Voie::getAngleVoisin(Voie *voisin) const
{
    return angleLiaison[ordreLiaison.key(voisin)];
}

qreal Voie::getNouvelAngle(Voie *voisin) const
{
    return normaliserAngle(angleLiaison[ordreLiaison.key(voisin)] + 180.0);
}

qreal Voie::getAngleDeg(int liaison) const
{
    return angleLiaison[liaison];
}

qreal Voie::getAngleRad(int liaison) const
{
    return (angleLiaison[liaison] /180.0) * PI;
}

void Voie::setAngleDeg(int liaison, qreal angle)
{
    qreal temp = angle * 4.0;

    if (fmod(temp, 1.0) < 0.5)
        temp = floor(temp) / 4.0;
    else
        temp = ceil(temp) / 4.0;

    angleLiaison[liaison] = normaliserAngle(temp);
}

void Voie::setAngleRad(int liaison, qreal angle)
{
    qreal temp = angle * (180.0 / PI) * 4.0;
    qreal temp1;

    if (fmod(temp, 1.0) < 0.5)
        temp1 = floor(temp) / 4.0;
    else
        temp1 = ceil(temp) / 4.0;

    qreal nouvel=normaliserAngle(temp1);
    angleLiaison[liaison] = normaliserAngle(nouvel);

}

Voie* Voie::getVoieVoisineDOrdre(int n)
{
    return ordreLiaison.value(n);
}

void Voie::drawBoundingRect(QPainter *
                            #ifdef DRAW_BOUNDINGRECT
                            painter
                            #endif
                            )
{
#ifdef DRAW_BOUNDINGRECT
    painter->setPen(Qt::DashLine);
    painter->drawRect(this->boundingRect());
#endif
}

void Voie::setIdVoie(int id)
{
    idVoie=id;
//    this->setToolTip(QString("Voie numéro: %1").arg(idVoie));
}

int Voie::getIdVoie()
{
    return idVoie;
}

/*
#include "commandetrain.h"
void Voie::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    char buf[100];
    sprintf(buf,"Rail numero: %d",this->idVoie);
    CommandeTrain::getInstance()->afficher_message(buf);
}
*/
