#ifndef SIMVIEW_H
#define SIMVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>

#include "connect.h"
#include "voie.h"
#include "voievariable.h"
#include "loco.h"
#include "segment.h"


class ExplosionItem :  public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
public:
    ExplosionItem(){};

};

class SimView : public QGraphicsView
{
    Q_OBJECT
public:
    /** Constructeur de classe
      *
      */
    explicit SimView(QWidget *);

    /** Permet d'ajouter une voie à la simulation.
      * \param v la voie à ajouter
      * \param ID le numéro de la voie
      */
    void addVoie(Voie* v, int ID);

    /** Permet d'ajouter une voie variable à la liste idoine de la simulation.
      * \param vv la voie variable à ajouter
      * \param ID le numéro de la voie variable
      * Remarque : une voie variable a donc deux numéros :
      *            - son numéro de voie
      *            - son numéro de voie variable
      */
    void addVoieVariable(VoieVariable* vv, int ID);

    /** Permet d'ajouter une contact à la simulation.
      * \param c le contact à ajouter
      * \param ID le numéro du contact
      */
    void addContact(Contact* c, int ID);

    /** Permet d'indiquer la première voie à poser, par rapport à laquelle
      * toutes les autres voies vont se positionner.
      * \param v le voie a poser en premier.
      */
    void setPremiereVoie(Voie* v);

    /** Permet de modifier une voie variable.
      * \param n le numéro de la voie variable.
      * \param v l'etat de la voie variable (DEVIE ou TOUT_DROIT)
      */
    void modifierAiguillage(int n, int v);

    /** Lance la construction de la maquette (placement des voies, etc...)
      */
    void construireMaquette();

    /** supprime toutes les voies, contacts, etc... en vue d'un nouveau chargement.
      */
    void viderMaquette();

    /** Génére la liste des segments de la maquette.
      *
      */
    void genererSegments();

    /** Ajoute une locomotive.
      * \param l la loco à ajouter.
      * \param ID le numéro de la loco.
      */
    void addLoco(Loco* l, int ID);

    /** Attribue des couleurs à chaque locomotive présente, de telle manière qu'il soit
      * le plus facile de les distinguer.
      * Remarque : la première loco est toujours rouge.
      */
    void peintLocos();

    /** permet de zoomer dans le vue.
      *
      */
    void zoomIn();

    /** permet de dézoomer de la vue.
      *
      */
    void zoomOut();

    /** ajuste le zoom de telle manière que la maquette soit entièrement affichée.
      *
      */
    void zoomFit();

    /** retourne le contact ayant le numéro n.
      * \param n le numéro du contact
      * \return le contact correspondant.
      */
    Contact* getContact(int n);

    /** raffraichit l'affichage.
      *
      */
    void redraw();
signals:

    /** Signale qu'une loco a changé de segment, et se trouve que le segment s.
      * \param s, le segment occupé.
      */
    void locoSurSegment(Segment* s);

    /** Signale le changment d'état d'une voie variable.
      * \param v la voie variable ayant changé.
      */
    void notificationVoieVariableModifiee(Voie* v);
public slots:

    /** effectue un nouveau pas d'animation.
      *
      */
    void animationStep();

    /** démarre l'animation
      *
      */
    void animationStart();

    /** stoppe l'animation
      *
      */
    void animationStop();

    /** prépare la locomotive au départ.
      * \param contactA le premier contact définissant le segment sur lequel se trouve la loco.
      * \param contactB le second contact définissant le segment sur lequel se trouve la loco.
      * \param numLoco le numéro de la loco à placer.
      * \param vitesseLoco la vitesse de la loco.
      */
    void setLoco(int contactA, int contactB, int numLoco, int vitesseLoco);

    /** pas implémenté.
      *
      */
    void askLoco(int, int);

    /** permet de changer la vitesse d'une loco.
      * \param numLoco le numéro de la loco à changer
      * \param vitesseLoco la nouvelle vitesse de la loco.
      */
    void setVitesseLoco(int numLoco, int vitesseLoco);

    /** Inverse le sens de la loco.
      * \param numLoco le numéro de la loco à inverser.
      */
    void reverseLoco(int numLoco);

    /** permet de changer la vitesse d'une loco.
      * \param numLoco le numéro de la loco à changer
      * \param vitesseLoco la nouvelle vitesse de la loco.
      */
    void setVitesseProgressiveLoco(int numLoco, int vitesseLoco);

    /** arrete la loco
      * \param numLoco le numéro de la loco.
      */
    void stopLoco(int numLoco);

    /** modifie l'etat d'une voie variable.
      * \param numVoieVariable le numéro de la voie variable.
      * \param direction la nouvelle direction de la voie (DEVIE ou TOUT_DROIT)
      */
    void setVoieVariable(int numVoieVariable, int direction);

    /** reçoit l'information qu'une loco a changé de segment.
      * \param ctc1 et ctc2 définissent le segment.
      * \param l la loco ayant changé de segment.
      */
    void locoSurNouveauSegment(Contact* ctc1, Contact* ctc2, Loco* l);

    /** reçoit l'information qu'une voie variable a été modifiée.
      * \param v la voie variable modifiée.
      */
    void voieVariableModifiee(Voie* v);


private:
    QTimer* timer;
    QGraphicsScene * scene;
    QMap<int, Voie*> Voies;
    QMap<int, VoieVariable*> VoiesVariables;
    QMap<int, Contact*> contacts;
    Voie* premiereVoie;
    QMap<int, Loco*> Locos;
    QList<Segment*> segments;

    /** retourne le segment correspondant à la paire de contacts passée en paramètre
      * \param contactA et contactB les contacts définissant les segment.
      * \return le segment correspondant.
      */
    Segment* getSegmentByContacts(int contactA, int contactB);

    bool checkLoco(int numLoco);

    bool checkVoieVariable(int numVoie);
};

#endif // SIMVIEW_H
