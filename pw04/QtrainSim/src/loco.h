#ifndef LOCO_H
#define LOCO_H

#include <QObject>
#include <QAbstractGraphicsShapeItem>
#include <QStaticText>
#include <QPainter>
#include <QTimer>

#include "general.h"
#include "voie.h"
#include "segment.h"
#include "connect.h"

class panneauNumLoco : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT
public:
    /** Constructeur de classe. Sert à afficher le numéro des locos.
      * \param numLoco, le numéro de la loco.
      */
    explicit panneauNumLoco(int numLoco, QObject *parent = 0);

    /** Retourne le rectangle englobant le panneau.
      * \return le rectangle englobant le panneau.
      */
    QRectF boundingRect() const;

    /** méthode d'affichage.
      *
      */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

    /** retourne le numéro de la loco.
      * \return le numéro de la loco.
      */
    int getNumLoco();
private:
    int numLoco;
};

class LocoCtrl;

class Loco : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT
public:

    /** Constructeur de classe.
      * \param numLoco le numéro de la loco.
      */
    explicit Loco(int numLoco, QObject *parent = 0);

    /** Permet de changer la vitesse de la loco.
      * Le comportement dépend de l'option "Inertie" :
      * avec l'inertie, le changement sera progressif.
      * sans l'inertie, le changement sera immédiat.
      * \param v la nouvelle vitesse de la loco.
      */
    void setVitesse(int v);

    /** Retourne la vitesse actuelle de la loco.
      * \return la vitesse actuelle de la loco.
      */
    int getVitesse();

    /** permet de changer la direction de la loco.
      * N'est pas utilisé : pour changer de sens, on effectue une rotation de 180°.
      * \param d la nouvelle direction (DIRECTION_LOCO_GAUCHE ou DIRECTION_LOCO_DROITE)
      */
    void setDirection(int d);

    /** retourne la direction actuelle de la loco
      * \return la direction actuelle de la loco
      */
    int getDirection();

    /** permet d'assigner la couleur principale de la loco.
      * \param r la valeur du rouge (0-255)
      * \param g la valeur du vert (0-255)
      * \param b la valeur du bleu (0-255)
      */
    void setCouleur(int r, int g, int b);

    /** retourne la couleur principale de la loco
      * \return la couleur principale de la loco
      */
    QColor getCouleur();

    /** Retourne le rectangle englobant la loco.
      * \return le rectangle englobant la loco.
      */
    QRectF boundingRect() const;

    /** Méthode d'affichage.
      *
      */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

    /** permet d'indiquer la voie sur laquelle la loco est posée.
      * \param v la voie actuelle.
      */
    void setVoie(Voie* v);

    /** retourne la voie sur laquelle est la loco.
      * \return la voie sur laquelle est la loco.
      */
    Voie* getVoie();

    /** permet d'indiquer la voie vers laquelle la loco est dirigée.
      * \param v la voie vers laquelle la loco est dirigée.
      */
    void setVoieSuivante(Voie* v);

    /** retourne la voie vers laquelle la loco est dirigée.
      * \return la voie vers laquelle la loco est dirigée.
      */
    Voie* getVoieSuivante();

    /** permet d'activer ou desactiver une loco. Une loco non active ne sera pas affichée.
      * \param active la valeur booléenne indiquant l'activation ou la desactivation de la loco.
      */
    void setActive(bool active);

    /** retourne la valeur booléenne indiquant l'activation ou la desactivation de la loco.
      * \return la valeur booléenne indiquant l'activation ou la desactivation de la loco.
      */
    bool getActive();

    /** effectue la transition d'une voie à l'autre et repositionne la loco (corrige les imprécisions de calcul).
      *
      */
    void avanceDUneVoie();

    /** Fait avancer la loco d'une certaine distance.
      * \param distance la distance de laquelle il faut faire avancer la loco.
      */
    void avancer(qreal distance);

    /** avance la loco le long d'une droite, d'une distance determinee.
      * \param distance la distance de laquelle il faut faire avancer la loco.
      */
    void avancerDroit(qreal distance);

    /** avance la loco selon une courbe, définie par un angle et un rayon.
      * \param angle l'angle de rotation
      * \param rayon le rayon de rotation
      */
    void avancerCourbe(qreal angle, qreal rayon);

    /** permet de mettre à jour l'angle cumule
      * \param a la nouvelle valeur de l'angle cumule
      */
    void setAngleCumule(qreal a);

    /** retourne l'angle cumule
      * \return l'angle cumule
      */
    qreal getAngleCumule();

    /** permet d'indiquer sur quel segment la loco se trouve
      * \param s le segment sur lequel la loco se trouve
      */
    void setSegmentActuel(Segment* s);

    /** permet de changer la valeur booléenne d'alerte de proximité.
      * \param b la nouvelle valeur booléenne d'alerte de proximité.
      */
    void setAlerteProximite(bool b);

    /** retourne la valeur booléenne d'alerte de proximité.
      * \return la valeur booléenne d'alerte de proximité.
      */
    bool getAlerteProximite();

    /** retourne le contour de la loco en coordonnées de la scene.
      * \return le contour de la loco en coordonnées de la scene.
      */
    QPolygonF getContour();

    /** Inverse le sens de la loco en conservant ou retrouvant la vitesse initiale.
      * Le comportement dépend de l'option "Inertie" :
      * avec l'inertie, le changement sera progressif.
      * sans l'inertie, le changement sera immédiat.
      */
    void inverserSens();

    /** Permet de corriger l'angle de la loco suite à des imprécisions de calcul.
      * \param nouvelAngle le nouvel angle.
      */
    void corrigerAngle(qreal nouvelAngle);

    LocoCtrl *controller;
signals:

    /** signale que la loco a atteint un nouveau segment
      * \param ctc1 le premier contact du segment
      * \param ctc2 le deuxieme contact du segment
      * \param l la loco emettrice du signal.
      */
    void nouveauSegment(Contact* ctc1, Contact* ctc2, Loco* l);

    /** Signale un déraillement.
      * \param l la loco emettrice du signal.
      */
    void deraillement(Loco* l);
public slots:

    /** Reçoit l'indication qu'une loco est sur le segment s.
      * \param s le segment signalé comme occupé.
      */
    void locoSurSegment(Segment* s);

    /** Reçoit l'indication qu'une voie variable a été modifiée.
      * \param v la voie variable modifiée.
      */
    void voieVariableModifiee(Voie* v);

    /** Reçoit le signal qu'il faut adapter la vitesse d'un incrément / décrément.
      */
    void adapterVitesse();
private:
    panneauNumLoco* numLoco1{nullptr};
    panneauNumLoco* numLoco2{nullptr};
    qreal angleCumule;
    bool active;
    int vitesse;
    int vitesseFuture;
    int direction;
    QColor couleur;
    Voie* voieActuelle{nullptr};
    Voie* voieSuivante{nullptr};
    Segment* segmentActuel{nullptr};
    bool alerteProximite;
    bool inverser;
    bool deraille;
    QTimer* timer{nullptr};
    QWaitCondition* VarCond{nullptr};
    QMutex* mutex{nullptr};
};

#endif // LOCO_H
