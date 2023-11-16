/*
 * Fichier          : ctrain_handler.c
 * Auteur           : Kevin Georgy
 *
 * Date de creation : 4.2.2009
 * But              : Implemente les fonctions de controle du simulateur/maquette de
 *                    trains.
 *                    Mappage en C des fonctions fournies par "train_handler.ads".
 * Revision         : 27.3.2009 (CEZ)
 */
 
#include "ctrain_handler.h"
#include "commandetrain.h"

#define CMD_TRAIN CommandeTrain::getInstance()

#ifndef MAQUETTE
/*
 * Initialise la communication avec la maquette/simulateur.
 * A appeler au debut du programme client.
 */
void init_maquette(void) {
    CMD_TRAIN->init_maquette();
}

/*
 * Met fin a la simulation, donc de stopper toute activite sur la maquette. A appeler a
 * la fin du programme client.
 */
void mettre_maquette_hors_service(void) {
    CMD_TRAIN->mettre_maquette_hors_service();
}

/*
 * Permet de retablir l'alimentation de la maquette, donc de reactive toute la maquette.
 * Elle n'a pas besoin d'etre appelee apres "init_maquette".
 */
void mettre_maquette_en_service(void) {
    CMD_TRAIN->mettre_maquette_en_service();
}

/*
 * Change la direction d'un aiguillage.
 *   no_aiguillage : No de l'aiguillage a diriger.
 *   direction     : Nouvelle direction. (DEVIE ou TOUT_DROIT)
 *   temps_alim    : Temps l'alimentation minimal du bobinage de l'aiguillage.
 */
void diriger_aiguillage(int no_aiguillage, int direction, int temps_alim) {
    CMD_TRAIN->diriger_aiguillage(no_aiguillage,direction,temps_alim);
}

/*
 * Attend l'activation du contact donne.
 *   no_contact : No du contact dont on attend l'activation.
 */
void attendre_contact(int no_contact) {
    CMD_TRAIN->attendre_contact(no_contact);
}

/*
 * Arrete une locomotive (met sa vitesse a VITESSE_NULLE).
 *   no_loco : No de la loco a arreter.
 */
void arreter_loco(int no_loco) {
    CMD_TRAIN->arreter_loco(no_loco);
}

/*
 * Change la vitesse d'une loco par palier.
 *   no_loco        : No de la loco a stopper.
 *   vitesse_future : Vitesse apres changement.
 * Remarque : Dans le simulateur, cette procedure agit comme la fonction
 *            "mettre_vitesse_loco". C'est-a-dire que l'acceleration est immediate
 *            (de la vitesse actuelle a la vitesse specifiee).
 */
void mettre_vitesse_progressive(int no_loco, int vitesse_future) {
    CMD_TRAIN->mettre_vitesse_progressive(no_loco,vitesse_future);
}

/*
 * Permet d'allumer ou d'eteindre les phares de la locomotive.
 *   no_loco : No de la loco a controler.
 *   etat    : Nouvel etat des phares. (ETEINT ou ALLUME)
 * Remarque : Dans le simulateur cette fonction n'a aucun effet.
 *            Les locomotive, representee par des rectangles, possedent une partie jaune
 *            indiquant le sens de deplacement. L'utilisation des phares n'est donc
 *            plus utile.
 */
void mettre_fonction_loco(int no_loco, char etat) {
    CMD_TRAIN->mettre_fonction_loco(no_loco,etat);
}

/*
 * Inverse le sens d'une locomotive, en conservant sa vitesse.
 *   no_loco : No de la loco a inverser.
 */
void inverser_sens_loco(int no_loco) {
    CMD_TRAIN->inverser_sens_loco(no_loco);
}

/*
 * Change la vitesse d'une loco.
 *   no_loco : No de la loco a controler.
 *   vitesse : Nouvelle vitesse.
 */
void mettre_vitesse_loco(int no_loco, int vitesse) {
    CMD_TRAIN->mettre_vitesse_loco(no_loco,vitesse);
}

/*
 * Indique au simulateur de demander une loco a l'utilisateur. L'utilisateur entre le
 * numero et la vitesse de la loco. Celle-ci est ensuite placee entre les contacts
 * "contact_a" et "contact_b".
 *   contact_a   : Contact vers lequel la loco va se diriger.
 *   contact_b   : Contact a l'arriere de la loco.
 *   numero_loco : Numero de loco choisi par l'utilisateur.
 *   vitesse     : Vitesse choisie par l'utilisateur.
 */
void demander_loco(int contact_a, int contact_b, int *no_loco, int *vitesse) {
    CMD_TRAIN->demander_loco(contact_a,contact_b,no_loco,vitesse);
}

#endif // MAQUETTE

/*
 * Indique au simulateur de demander une loco a l'utilisateur. L'utilisateur entre le
 * numero et la vitesse de la loco. Celle-ci est ensuite placee entre les contacts
 * "contact_a" et "contact_b".
 *   contact_a   : Contact vers lequel la loco va se diriger.
 *   contact_b   : Contact a l'arriere de la loco.
 *   numero_loco : Numero de loco choisi par l'utilisateur.
 *   vitesse     : Vitesse choisie par l'utilisateur.
 */
void assigner_loco(int contact_a, int contact_b, int no_loco, int vitesse) {
    CMD_TRAIN->assigner_loco(contact_a,contact_b,no_loco,vitesse);
}


void selection_maquette(const char *maquette)
{
    CMD_TRAIN->selection_maquette(maquette);
}

void afficher_message(const char *message)
{
    CMD_TRAIN->afficher_message(message);
}


void afficher_message_loco(int numLoco,const char* message)
{
    CMD_TRAIN->afficher_message_loco(numLoco,message);
}

const char *getCommand()
{
    static QByteArray cmd;

    cmd = CMD_TRAIN->getCommand().toLocal8Bit();
    return cmd.data();
}

void getCommandInArray(char *commande, int taille)
{
    QByteArray cmd(CMD_TRAIN->getCommand().toLocal8Bit());
    strncpy(commande, cmd.data(), taille - 1);
    commande[taille - 1] = '\0';
}
