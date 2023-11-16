#ifndef VOIE_H
#define VOIE_H

#include <math.h>

#include <QObject>
#include <QList>
#include <QMap>
#include <QPointF>
#include <QDebug>
#include <QRectF>
#include <QAbstractGraphicsShapeItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QMap>

#include "general.h"
#include "contact.h"

class Voie : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT

public:
    /** Constructeur de classe.
      */
    Voie();

    /** Méthode permettant de calculer la position de la voie, en fonction d'une voie
      * voisine déjà posée. S'il s'agit de la première voie posée, on lui attribue une position
      * par défaut.
      * \param v pointeur sur la voie voisine déjà posée.
      */
    void calculerPosition(Voie* v = nullptr);

    /** méthode virtuelle visant à calculer les angles et coordonnées (locales) de chaque extrémité
      * de la voie.
      * Cette méthode est appelée récursivement, jusqu'à ce que toutes les voies aient été orientées.
      * \param v pointeur sur la voie voisine déjà orientée.
      */
    virtual void calculerAnglesEtCoordonnees(Voie* v = nullptr) = 0;

    /** permet de lier deux voies, c'est-à-dire d'indiquer qu'elles doivent se connecter l'une à l'autre,
      * et sur quelle extrémité.
      * \param v pointeur sur la voie à lier.
      * \param ordre indique l'extrémité sur laquelle v doit se fixer.
      */
    void lier(Voie* v, int ordre);

    /** permet de savoir si la voie est orientée, à savoir si la méthode calculerAnglesEtCoordonnees(...)
      * a déjà été appelée.
      * \return vrai si la voie est déjà orientée, faux sinon.
      */
    bool estOrientee();

    /** permet de savoir si la voie est posée, à savoir si la méthode calculerPosition(...)
      * a déjà été appelée.
      * \return vrai si la voie est déjà posée, faux sinon.
      */
    bool estPosee();

    /** retourne la position en coordonnées absolue de l'extrémité de la voie reliée à la voie passée en paramètre.
      * \param v la voie reliée à l'extrémité dont la position est désirée.
      * \return la position absolue de l'extrémité.
      */
    QPointF getPosAbsLiaison(Voie* v);

    /** attribue le contact passé en paramètre à la voie.
      * \param c le contact attribué.
      */
    void setContact(Contact* c);

    /** retourne le contact de la voie. retourne nullptr s'il n'y a pas de contact.
      * \return pointeur sur le contact de la voie.
      */
    Contact* getContact() const;

    /** positionne le contact correctement au milieu de la voie.
      *
      */
    virtual void calculerPositionContact()=0;

    /** Permet l'exploration récursive des voies en vue de la création des segments.
      * \param voieAppelante pointeur sur la voie appelant récursivement celle-ci.
      * \return Une liste de liste de voies, correspondant à tous chemins partant de cette voie
      *         et terminant sur un contact, sauf le chemin partant depuis la voie appelante.
      */
    virtual QList<QList<Voie*>*> explorationContactAContact(Voie* voieAppelante)=0;

    /** Démarre l'exploration contact à contact en vue de la création des segments.
      * \return Une liste de liste de voies, correspondant à tous chemins partant de cette voie
      *         et terminant sur un contact.
      */
    QList<QList<Voie*>*> startExplorationContactAContact();

    /** retourne le nombre de liaisons (en d'autres termes d'extrémités) de la voie.
      * \return le nombre de liaisons de la voie.
      */
    int getNbreLiaisons() const;

    /** retourne la valeur X de l'extrémité la plus à gauche de la voie.
      * \return la valeur X de l'extrémité la plus à gauche de la voie.
      */
    qreal getXmin() const;

    /** retourne la valeur X de l'extrémité la plus à droite de la voie.
      * \return la valeur X de l'extrémité la plus à droite de la voie.
      */
    qreal getXmax() const;

    /** retourne la valeur Y de l'extrémité la plus haute de la voie.
      * \return la valeur Y de l'extrémité la plus haute de la voie.
      */
    qreal getYmin() const;

    /** retourne la valeur Y de l'extrémité la plus basse de la voie.
      * \return la valeur Y de l'extrémité la plus basse de la voie.
      */
    qreal getYmax() const;

    /** permet de connaitre l'angle de l'extrémité connectant cette voie à la voisine.
      * \param voisin la voie voisine.
      * \return l'angle de liaison en degrés.
      */
    qreal getAngleVoisin(Voie* voisin) const;

    /** permet de connaitre l'angle réajusté lorsqu'une locomotive arrive sur une voie.
      * \param voisin la voie d'ou vient la loco.
      * \return l'angle ajusté de la loco.
      */
    qreal getNouvelAngle(Voie* voisin) const;

    /** retourne l'angle en degrés de l'extrémité d'ordre spécifié en paramètre.
      * \param liaison l'ordre de l'extrémité dont on veut l'angle.
      * \return l'angle en degrés.
      */
    qreal getAngleDeg(int liaison) const;

    /** retourne l'angle en radians de l'extrémité d'ordre spécifié en paramètre.
      * \param liaison l'ordre de l'extrémité dont on veut l'angle.
      * \return l'angle en radians.
      */
    qreal getAngleRad(int liaison) const;

    /** permet de fixer l'angle de l'extrémité d'ordre spécifié en paramétre,
      * en passant un angle en degrés.
      * \param liaison l'ordre de l'extrémité
      * \param l'angle en degrés.
      */
    void setAngleDeg(int liaison, qreal angle);

    /** permet de fixer l'angle de l'extrémité d'ordre spécifié en paramétre,
      * en passant un angle en radians.
      * \param liaison l'ordre de l'extrémité
      * \param l'angle en radians.
      */
    void setAngleRad(int liaison, qreal angle);

    /** retourne la longueur a parcourir pour traverser la voie.
      * \return la longueur a parcourir pour traverser la voie.
      */
    virtual qreal getLongueurAParcourir()=0;

    /** retourne la voie suivante, en fonction de la voie d'arrivee.
      * \param voieArrivee la voie d'arrivee
      * \return le voie suivante.
      */
    virtual Voie* getVoieSuivante(Voie* voieArrivee)=0;

    /** fournit les informations nécessaires a l'avancement de la locomotive d'une certaine distance.
      * \param dist la distance à parcourir
      * \param angle l'angle de rotation
      * \param rayon le rayon de rotation
      * \param angleCumule l'angle actuel de la loco
      * \param posActuelle la position actuelle de la loco en coordonnees absolues.
      * \param voieSuivante la voie vers laquelle se dirige la loco.
      */
    virtual void avanceLoco(qreal &dist, qreal &angle, qreal &rayon, qreal angleCumule, QPointF posActuelle, Voie* voieSuivante)=0;

    /** retourne la voie voisine spécifiée par son ordre.
      * \param n l'ordre de la voie
      * \return la voie d'ordre n.
      */
    Voie* getVoieVoisineDOrdre(int n);

    /** permet de spécifier la manière dont la voie sera dessinée.
      * \param color la couleur de la voie.
      */
    void setNewPen(const QColor &color);

    /** permet, pour les voies variables, de modifier leur etat.
      * \param nouvelEtat le nouvel etat de la voie variable.
      */
    virtual void setEtat(int nouvelEtat)=0;

    /** permet de corriger la position de l'extrémité liant cette voie à la voie v.
      * \param deltaX la correction en X
      * \param deltaY la correction en Y
      * \param v la voie a l'extrémité à corriger.
      */
    virtual void correctionPosition(qreal deltaX, qreal deltaY, Voie* v)=0;

    /** permet d'afficher le rectangle englobant la voie. Utile pour le débuggage.
      * \param painter l'outil de dessin.
      */
    void drawBoundingRect(QPainter *painter);

    /** permet de corriger la position d'une locomotive par rapport à une voie.
      * \param x la correction en X
      * \param y la correction en Y
      */
    virtual void correctionPositionLoco(qreal &x, qreal &y)=0;

    void setIdVoie(int id);

    int getIdVoie();
protected:
    QMap<int, Voie*> ordreLiaison;
    QMap<int, QPointF*> coordonneesLiaison;
    bool orientee, posee;

    /** normalise l'angle entre 0 et 360 degrés.
      * \param angle l'angle à normaliser
      * \return l'angle normalisé
      */
    double normaliserAngle(double angle) const;
    QPointF* position;
    QRectF* bRect;
    Contact* contact;
    int idVoie;

    //virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
private:
    QMap<int, qreal> angleLiaison;
};

#endif // VOIE_H
