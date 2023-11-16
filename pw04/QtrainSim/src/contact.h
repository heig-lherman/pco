#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QAbstractGraphicsShapeItem>
#include <QMutex>
#include <QWaitCondition>
#include <QPainter>
#include <QDebug>
#include <math.h>

#include "general.h"

class Contact : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT
public:
    /** Constructeur de Classe.
      * \param numContact le numero du contact.
      * \param numVoiePorteuse le numéro de la voie porteuse du contact.
      * \param parent le parent du contact. Pas utilisé à la construction.
      */
    explicit Contact(int numContact, int numVoiePorteuse, QObject *parent = 0);

    /** Méthode bloquante, permettant d'attendre sur l'activation du contact.
      */
    void attendContact();

    /** Méthode appelée quand une loco passe sur le contact.
      * Libère les threads en attente.
      */
    void active();

    /** retourne le numéro de la voie porteuse.
      * \return le numéro de la voie porteuse.
      */
    int getNumVoiePorteuse();

    /** Permet de spécifier selon quel angle le numéro du contact doit être décalé.
      * \param angle l'angle en radians.
      */
    void setAngle(qreal angle);

    /** retourne le rectangle englobant le contact. Nécessaire à l'affichage.
      * \return le rectangle englobant le contact.
      */
    QRectF boundingRect() const;

    /** Méthode d'affichage. Appelée par QGraphicsView lors du dessin de la fenetre.
      * pas besoin de se préoccuper des parametres.
      */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

    /** retourne le numéro du contact.
      * \return le numéro du contact.
      */
    int getNumContact();
signals:

public slots:

private:
    int numVoiePorteuse;
    int numContact;
    QWaitCondition* VarCond;
    QMutex* mutex;
    qreal angle;
    bool waitingOn;
};

#endif // CONTACT_H
