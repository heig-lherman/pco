#ifndef GENERAL_H
#define GENERAL_H

/**
    Définitions de constantes.

    /!\ Lors de modifications, il faut recompiler l'entier du programme.
        Une simple compilation ne suffit pas.
 */

#define CMD_TRAIN CommandeTrain::getInstance()

#define PI 3.14159265358979323846264338327950288419717
#define MARGE_BOUNDING_RECT 5.0

//! Dimensions des locos
#define LONGUEUR_LOCO 120.0
#define LARGEUR_LOCO 30.0
#define LONGUEUR_FEUX 30.0
#define DIRECTION_LOCO_GAUCHE 1
#define DIRECTION_LOCO_DROITE -1
//! Inertie des locos. indiqué en millièmes de secondes entre chaque changement
//! de la valeur de vitesse de 1.
#define INERTIE_LOCO 100

//! NE PAS CHANGER!!! nécessaire au calcul des poses de voies.
#define DIRECTION_VOIE_GAUCHE 1.0
#define DIRECTION_VOIE_DROITE -1.0

//! taille, couleur des contacts, et distance entre le contact et son numéro.
#define TAILLE_CONTACT 15.0
#define COULEUR_CONTACT Qt::blue
#define COULEUR_CONTACT_WAITING Qt::green
#define TRANSLATION_NUM_CONTACT 60.0
#define FONTE_CONTACT QFont("Verdana", 30, 99)
#define COULEUR_FONTE_CONTACT QColor(255,127,0)

//! indications de l'ordre d'affichage des éléments.
//! (les objets ayant de grandes valeurs de zval sont affichés dessus)
#define ZVAL_VOIE 1.0
#define ZVAL_CONTACT 2.0
#define ZVAL_EXPLOSION 4
#define ZVAL_LOCO 3.0

//! indication du nombre d'images à calculer par seconde.
//! en cas de fort ralentissement, baisser cette valeur.
//! Valeurs conseillées : 30-60.
#define FRAME_RATE 60

//! permet d'ajuster la vitesse des locos. Ne pas changer.
#define FACTEUR_VITESSE 0.05

//! Couleurs des voies.
#define COULEUR_DROITE            QColor(Qt::black)
#define COULEUR_COURBE            QColor(Qt::black)
#define COULEUR_AIGUILLAGE        QColor(Qt::black)
#define COULEUR_BUTTOIR           QColor(Qt::black)
#define COULEUR_CROISEMENT        QColor(Qt::black)
#define COULEUR_TRAVERSEEJONCTION QColor(Qt::black)

//! largeur des voies.
#define LARGEUR_VOIE     20

//! Vitesse a l'arret
#define	VITESSE_NULLE 0

//! Vitesse minimum
#define	VITESSE_MINIMUM 3

//! Vitesse maximum
#define	VITESSE_MAXIMUM 14

//! Numero max. d'aiguillage
#define	MAX_AIGUILLAGES 80

//! Numero max. de contact
#define MAX_CONTACTS 64

//! Numero max. de loco
#define	MAX_LOCOS 80

//! Direction des aiguillages
#define DEVIE 0
#define TOUT_DROIT 1

//! Etat des phares
#define ETEINT 0
#define ALLUME 1


#define DATADIR QCoreApplication::applicationDirPath()+"/data"

#define MAQUETTE_DIR DATADIR+"/Maquettes"

#endif // GENERAL_H
