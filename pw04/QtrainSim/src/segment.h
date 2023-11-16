#ifndef SEGMENT_H
#define SEGMENT_H

#include <QObject>
#include <QDebug>

#include "contact.h"
#include "voie.h"

class Segment : public QObject
{
    Q_OBJECT
public:
    /** Constructeur de classe
      * \param c1 le premier contact du segment
      * \param c2 le second contact du segment
      * \param lv la liste des voies du segment
      */
    explicit Segment(Contact* c1, Contact* c2, QList<Voie*> lv, QObject *parent = 0);

    /** retourne la voie droite ou courbe la plus au milieu du segment.
      * \return la voie droite ou courbe la plus au milieu du segment.
      */
    Voie* getMilieu();

    /** retourne la voie suivant la voie droite ou courbe la plus au milieu du segment.
      * \return la voie suivant la voie droite ou courbe la plus au milieu du segment.
      */
    Voie* getSuivantMilieu();

    /** retourne la voie precedant la voie droite ou courbe la plus au milieu du segment.
      * \return la voie precedant la voie droite ou courbe la plus au milieu du segment.
      */
    Voie* getPrecedentMilieu();

    /** permet de vérifier si le segment relie les deux contacts passés en paramètre.
      * \param c1 le premier contact.
      * \param c2 le second contact.
      * \return vrai si le segment relie c1 et c2, faux sinon.
      */
    bool relie(Contact* c1, Contact* c2);
signals:

public slots:

private:
    Contact* contact1;
    Contact* contact2;
    QList<Voie*> voies;
};

#endif // SEGMENT_H
