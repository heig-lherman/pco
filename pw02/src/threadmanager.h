    /**
  \file threadmanager.h
  \author Yann Thoma
  \date 24.02.2017
  \brief Classe pour reverser un hash md5 par brute force.


  Ce fichier contient la définition de la classe ThreadManager, qui permet de
  reverser un hash md5 par brute force.
*/

#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QObject>
#include <QString>


/**
 * \brief The ThreadManager class
 *
 * Cette classe tente de reverser un hash md5 par brute force.
 */
class ThreadManager: public QObject
{
    Q_OBJECT
private:

public:
    /**
     * \brief ThreadManager Constructeur simple
     * \param parent Objet parent de l'interface
     */
    ThreadManager(QObject *parent);

    /**
     * \brief startHacking tâche qui s'occupe de trouver le hash md5.
     * \param charset QString tous les caractères possibles composant le mot de
     * passe
     * \param salt QString sel qui permet de modifier dynamiquement le hash
     * \param hash QString hash à reverser
     * \param nbChars taille du mot de passe
     * \param nbThreads nombre de threads qui doivent reverser le hash
     * \return Le hash trouvé, ou une chaine vide sinon
     *
     * Cette fonction exécute réellement la recherche.
     */
    QString startHacking(
            QString charset,
            QString salt,
            QString hash,
            unsigned int nbChars,
            unsigned int nbThreads
    );


    /**
     * \brief incrementPercentComputed fonction qui indique que le pourcentage
     * de test effectué pour casser le hash md5.
     * \param percentComputed double pourcentage de tests effectués pour
     * reverser le hash md5
     */
    void incrementPercentComputed(double percentComputed);

signals:
    /**
     * \brief sig_incrementPercentComputed signal qui indique que le pourcentage
     * de test effectué pour casser le hash md5.
     * \param percentComputed double pourcentage de tests effectués pour
     * reverser le hash md5
     */
    void sig_incrementPercentComputed(double percentComputed);
};

#endif // THREADMANAGER_H
