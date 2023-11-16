#ifndef COMMANDETRAIN_H
#define COMMANDETRAIN_H

#include <QObject>
#include <QString>
#include <QMutex>
#include <QWaitCondition>

#include "general.h"

/**
  Toutes les methodes de cette classe doivent être reentrantes!!!!!!!
  */
class CommandeTrain : public QObject
{
    Q_OBJECT
protected:
    /**
      Constructeur vide.
      */
    CommandeTrain();

    /**
      Destructeur
      */
    ~CommandeTrain();

public:

    /**
     * Retourne l'unique instance de la commande de train.
     * Si le singleton n'a pas encore ete cree il l'est de maniere automatique.
     */
    static CommandeTrain *getInstance();



    /**
     * Initialise la communication avec la maquette/simulateur.
     * A appeler en debut de programme client.
     */
    void init_maquette(void);

    /**
     * Met fin a la simulation. A appeler en fin de programme client
     */
    void mettre_maquette_hors_service(void);

    /**
     * Realimente la maquette. Inutile apres init_maquette
     */
    void mettre_maquette_en_service(void);

    /**
     * Ajoute une loco dans la simulation.
     * Remarque : n'a pas d'effet sur la maquette reelle.
     * Remarque bis : Cette méthode est appelée automatiquement lors de l'appel de
     *                assigner_loco(...);
     * \param no_loco Numéro de la locomotive.
     */
    void ajouter_loco(int no_loco);

    /**
     * Change la direction d'un aiguillage.
     * \param no_aiguillage  No de l'aiguillage a diriger.
     * \param direction      Nouvelle direction. (DEVIE ou TOUT_DROIT)
     * \param temps_alim     Temps l'alimentation minimal du bobinage de l'aiguillage.
     * Remarque : temps_alim n'a pas d'effet dans le simulateur.
     */
    void diriger_aiguillage(int no_aiguillage, int direction, int);

    /**
     * Méthode bloquante, permettant d'attendre l'activation du contact voulu.
     * Remarque : le contact peut être activé par n'importe quelle locomotive.
     * \param no_contact  Numéro du contact dont on attend l'activation.
     */
    void attendre_contact(int no_contact);

    /**
     * Arrete une locomotive (met sa vitesse à  VITESSE_NULLE).
     * \param no_loco  Numéro de la loco à  stopper.
     */
    void arreter_loco(int no_loco);

    /**
     * Change la vitesse d'une loco par palier.
     * \param no_loco         No de la loco a stopper.
     * \param vitesse_future  Vitesse apres changement.
     *
     * Remarque : Dans le simulateur cette procedure agit comme la
     *            fonction "Mettre_Vitesse_Loco". Le comportement
     *            dépend de l'option "Inertie" dans le menu ad hoc.
     */
    void mettre_vitesse_progressive(int no_loco, int vitesse_future);

    /**
     * Permettre d'allumer ou d'eteindre les phares de la locomotive.
     * \param no_loco  No de la loco a controler.
     * \param etat     Nouvel etat des phares. (ETEINT ou ALLUME)
     * Remarque : dans le simulateur, les phares sont toujours allumés.
     *            Cette méthode n'a pas d'effet.
     */
    void mettre_fonction_loco(int, char);

    /**
     * Inverse le sens d'une locomotive, en conservant ou retrouvant sa vitesse originale.
     * \param no_loco  Numéro de la loco à inverser.
     * Remarque : Le comportement de la loco dépend du paramètre "Inertie"
     *            dans le menu ad hoc.
     */
    void inverser_sens_loco(int no_loco);

    /**
     * Change la vitesse d'une loco.
     * \param no_loco  No de la loco à contrôler.
     * \param vitesse  Nouvelle vitesse.
     */
    void mettre_vitesse_loco(int no_loco, int vitesse);

    /**
     * Indique au simulateur de demander une loco à l'utilisateur. L'utilisateur
     * entre le numero et la vitesse de la loco. Celle-ci est ensuite placee entre
     * les contacts "contact_a" et "contact_b".
     * \param contact_a    Contact vers lequel la loco va se diriger.
     * \param contact_b    Contact à l'arriere de la loco.
     * \param numero_loco  Numero de loco choisi par l'utilisateur.
     * \param vitesse      Vitesse choisie par l'utilisateur.
     * Remarque : Cette méthode n'est pas implémentée dans le simulateur.
     *            Veuillez utiliser la méthode assigner_loco(...);
     */
    void demander_loco(int contact_a, int contact_b, int *, int *);


    /**
     * Indique au simulateur que la loco d'identifiant no_loco dont être utilisée.
     * La méthode indique également entre quels contacts elle se trouve, ainsi que sa vitesse.
     * \param contact_a  Identifiant du contact vers lequel la loco va se diriger.
     * \param contact_b  Identifiant du contact à l'arrière de la loco.
     * \param no_loco    Numéro de la loco choisie.
     * \param vitesse    Vitesse à laquelle la loco devra se déplacer.
     */
    void assigner_loco(int contact_a,int contact_b,int no_loco,int vitesse);

    /**
      * Sélectionne la maquette à  utiliser.
      * Cette fonction termine l'application si la maquette n'est pas trouvée.
      * \param maquette Nom de la maquette.
      */
    void selection_maquette(QString maquette);

    void afficher_message(const char *message);

    void afficher_message_loco(int numLoco,const char *message);

    QString getCommand();

public slots:
    void commandSent(QString command);

protected slots:
    void timerTrigger();

signals:
    void addLoco(int no_loco);
    void setLoco(int contactA, int contactB, int numLoco, int vitesseLoco);
    void askLoco(int contactA, int contactB);
    void setVitesseLoco(int numLoco, int vitesseLoco);
    void reverseLoco(int numLoco);
    void setVitesseProgressiveLoco(int numLoco, int vitesseLoco);
    void stopLoco(int numLoco);
    void setVoieVariable(int numVoieVariable, int direction);
    void selectMaquette(QString maquette);
    void afficheMessage(QString message);
    void afficheMessageLoco(int numLoco,QString message);

private:
    QString command;
    QWaitCondition* VarCond;
    QMutex* mutex;
    bool waitingOn;
};

#endif // COMMANDETRAIN_H
