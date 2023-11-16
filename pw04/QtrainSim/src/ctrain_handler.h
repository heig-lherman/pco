#ifndef H_CTRAIN_HANDLER
#define H_CTRAIN_HANDLER

/*
 * Fichier          : ctrain_handler.h
 * Auteur           : Kevin Georgy
 *
 * Date de creation : 4.2.2009
 * But              : Fournit les fonctions de controle du simulateur/maquette de trains.
 * Revision         : 27.3.2009 (CEZ)
 *                    27.4.2009 (KGY) Ajout du extern "CE pour les applications c++
 *                    08.9.2011 (Jeremie Ecoffey) Adaptation au nouveau simulateur.
 *                    15.2.2012 (YTA) Ajout des fonctions pour affichage de messages dans
 *                              la console generale et les consoles des locos.
 */

#ifdef __cplusplus
extern "C" {
#endif

// Maquette A
#define MAQUETTE_A "MAQUET_A"

// Maquette B
#define MAQUETTE_B "MAQUET_B"

// Vitesse a l'arret
#define	VITESSE_NULLE 0

// Vitesse minimum
#define	VITESSE_MINIMUM 3

// Vitesse maximum
#define	VITESSE_MAXIMUM 14

// Numero max. d'aiguillage
#define	MAX_AIGUILLAGES 80

// Numero max. de contact
#define MAX_CONTACTS 64

// Numero max. de loco
#define	MAX_LOCOS 80

// Direction des aiguillages
#define DEVIE 0
#define TOUT_DROIT 1

// Etat des phares
#define ETEINT 0
#define ALLUME 1

/*
 * Initialise la communication avec la maquette/simulateur.
 * A appeler au debut du programme client.
 */
void init_maquette(void);

/*
 * Met fin a la simulation. A appeler a la fin du programme client.
 */
void mettre_maquette_hors_service(void);

/*
 * Realimente la maquette. Inutile apres init_maquette().
 */
void mettre_maquette_en_service(void);

/*
 * Change la direction d'un aiguillage.
 *   no_aiguillage : No de l'aiguillage a diriger.
 *   direction     : Nouvelle direction. (DEVIE ou TOUT_DROIT)
 *   temps_alim    : Temps l'alimentation minimal du bobinage de l'aiguillage.
 */
void diriger_aiguillage(int no_aiguillage, int direction, int temps_alim);

/*
 * Attend l'activation du contact donne.
 *   no_contact : No du contact dont on attend l'activation.
 */
void attendre_contact(int no_contact);

/*
 * Arrete une locomotive (met sa vitesse a VITESSE_NULLE).
 *   no_loco : No de la loco a arreter.
 */
void arreter_loco(int no_loco);

/*
 * Change la vitesse d'une loco par palier.
 *   no_loco        : No de la loco a stopper.
 *   vitesse_future : Vitesse apres changement.
 * Remarque : Dans le simulateur cette procedure agit comme la fonction
 *            "mettre_vitesse_loco". Son comportement depend de l'option
 *            "Inertie" dans le menu ad hoc.
 */
void mettre_vitesse_progressive(int no_loco, int vitesse_future);

/*
 * Permettre d'allumer ou d'eteindre les phares de la locomotive.
 *   no_loco : No de la loco a controler.
 *   etat    : Nouvel etat des phares. (ETEINT ou ALLUME)
 * Remarque : Dans le simulateur cette fonction n'a aucun effet.
 *            Les phares sont toujours allumes, et indiquent le sens de la loco.
 */
void mettre_fonction_loco(int no_loco, char etat);

/*
 * Inverse le sens d'une locomotive, en conservant ou retrouvant sa vitesse originale.
 *   no_loco : No de la loco a inverser.
 * Remarque : Dans le simulateur, le comportement depend de l'option "Inertie" dans le menu ad hoc.
 */
void inverser_sens_loco(int no_loco);

/*
 * Change la vitesse d'une loco.
 *   no_loco : No de la loco a controler.
 *   vitesse : Nouvelle vitesse.
 * Remarque : Dans le simulateur, le comportement depend de l'option "Inertie" dans le menu ad hoc.
 */
void mettre_vitesse_loco(int no_loco, int vitesse);

/*
 * Indique au simulateur de demander une loco a l'utilisateur. L'utilisateur entre le
 * numero et la vitesse de la loco. Celle-ci est ensuite placee entre les contacts
 * "contact_a" et "contact_b".
 *   contact_a   : Contact vers lequel la loco va se diriger.
 *   contact_b   : Contact a l'arriere de la loco.
 *   numero_loco : Numero de loco choisi par l'utilisateur.
 *   vitesse     : Vitesse choisie par l'utilisateur.
 * Remarque : cette methode n'est pas utilisee dans le simulateur.
 *            Veuillez utiliser assigner_loco(...);
 */
void demander_loco(int contact_a, int contact_b, int *no_loco, int *vitesse);

/*
 * Indique au simulateur d'assigner une loco.
 * Le numero et la vitesse de la loco sont definies, et elle est ensuite placee
 * entre les contacts "contact_a" et "contact_b".
 *   contact_a   : Contact vers lequel la loco va se diriger.
 *   contact_b   : Contact a l'arriere de la loco.
 *   numero_loco : Numero de loco.
 *   vitesse     : Vitesse de la loco.
 */
void assigner_loco(int contact_a, int contact_b, int no_loco, int vitesse);


/*
 * Selectionne la maquette a utiliser.
 * Cette fonction termine l'application si la maquette n'est pas trouvee.
 * La maquette est cherchee dans le repertoire contenant les maquettes.
 *   maquette : Nom de la maquette.
 */
void selection_maquette(const char *maquette);

/*
 * Affiche un message dans la console principale
 *   message : chaine de caractere qui sera affichee dans la console.
 */
void afficher_message(const char* message);

/*
 * Affiche un message dans la console d'une loco
 *   numLoco : numero de la locomotive
 *   message : chaine de caractere qui sera affichee dans la console.
 */
void afficher_message_loco(int numLoco,const char* message);

/*
 * Fonction bloquante permettant de recevoir la prochaine commande
 * entree par l'utilisateur.
 *   return : la commande entree par l'utilisateur
 */
const char* getCommand();

/*
 * Copie le résultat de la commande saisie par l'utilisateur dans le
 * tableau passé en paramètre
 *   commande : tableau de caractères
 *   taille : taille du tableau
 */
void getCommandInArray(char *commande, int taille);

#ifdef __cplusplus
}
#endif

#endif

