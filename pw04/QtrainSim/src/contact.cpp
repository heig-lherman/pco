#include "contact.h"
#include "trainsimsettings.h"

/** Constructeur de classe Contact.
  * @param numContact, le numero du contact.
  * @param numVoiePorteuse, le numero de la voie porteuse du contact.
  * @param parent, l'objet creant le contact (pas utilisé pour le moment).
  */

Contact::Contact(int numContact, int numVoiePorteuse, QObject *parent) :
    QObject(parent)
{
    this->numContact = numContact;
    this->numVoiePorteuse = numVoiePorteuse;
    mutex = new QMutex();
    VarCond = new QWaitCondition();
    setZValue(ZVAL_CONTACT);
    waitingOn=false;
}

int Contact::getNumContact()
{
    return numContact;
}


void Contact::attendContact()
{
    mutex->lock();
    waitingOn=true;
    update();
    VarCond->wait(mutex);
    waitingOn=false;
    update();
    mutex->unlock();
}

void Contact::active()
{
    VarCond->wakeAll();
}

int Contact::getNumVoiePorteuse()
{
    return this->numVoiePorteuse;
}

void Contact::setAngle(qreal angle)
{
    this->angle = angle;
}

QRectF Contact::boundingRect() const
{
    return QRectF(- 100.0, - 100.0, 200.0, 200.0);
}

#include <QGraphicsScene>

void Contact::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    if (waitingOn)
    {
        painter->setPen(COULEUR_CONTACT_WAITING);
        painter->setBrush(COULEUR_CONTACT_WAITING);
    }
    else
    {
        painter->setPen(COULEUR_CONTACT);
        painter->setBrush(COULEUR_CONTACT);
    }
    painter->drawEllipse(QPointF(0.0,0.0),TAILLE_CONTACT, TAILLE_CONTACT);

    if (TrainSimSettings::getInstance()->getViewContactNumber())
    {
        QString t;
        t.setNum(numContact);

        if (waitingOn)
        {
            painter->setPen(COULEUR_CONTACT_WAITING);
            painter->setFont(FONTE_CONTACT);
        }
        else
        {
            painter->setPen(COULEUR_FONTE_CONTACT);
            painter->setFont(FONTE_CONTACT);
        }
        qreal theangle=angle;
        while (theangle>PI)
             theangle-=PI;
        while (theangle<0.0)
             theangle+=PI;

        QRectF r=QRectF(TRANSLATION_NUM_CONTACT * cos(theangle) - 3.0 * TAILLE_CONTACT,
                        TRANSLATION_NUM_CONTACT * sin(theangle) - 3.0 * TAILLE_CONTACT,
                        6.0 * TAILLE_CONTACT,
                        6.0 * TAILLE_CONTACT);
/*
        r.translate(this->scenePos());
        if (this->scene()->items(r,Qt::IntersectsItemShape,Qt::AscendingOrder).size()>=2) {
            theangle+=PI;
            r=QRectF(TRANSLATION_NUM_CONTACT * cos(theangle) - 3.0 * TAILLE_CONTACT,
                                    TRANSLATION_NUM_CONTACT * sin(theangle) - 3.0 * TAILLE_CONTACT,
                                    6.0 * TAILLE_CONTACT,
                                    6.0 * TAILLE_CONTACT);
        }
        else
            r.translate(-this->scenePos());
  */
        painter->drawText(r,
                          t,
                          QTextOption(Qt::AlignHCenter|Qt::AlignVCenter));
        painter->drawLine(QPointF(0.0,0.0), QPointF(TRANSLATION_NUM_CONTACT * cos(theangle) / 2.0, TRANSLATION_NUM_CONTACT * sin(theangle) / 2.0));
    }
}
