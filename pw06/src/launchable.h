//     ____  __________     ___   ____ ___  _____ //
//    / __ \/ ____/ __ \   |__ \ / __ \__ \|__  / //
//   / /_/ / /   / / / /   __/ // / / /_/ / /_ <  //
//  / ____/ /___/ /_/ /   / __// /_/ / __/___/ /  //
// /_/    \____/\____/   /____/\____/____/____/   //
// Auteur : Rick Wertenbroek
// Rien à modifier ici

#ifndef LAUNCHABLE_H
#define LAUNCHABLE_H

#include <QDebug>

#include <pcosynchro/pcothread.h>

/*!
 * \brief La classe Launchable est une classe abstraite qui représente le fait d'avoir un thread
 * associé qui permet d'être lancé, thread qui exécute la fonction run() qui représente le
 * comportement de la classe qui la définit.
 */
class Launchable
{
public:
    Launchable() {}

    /*!
     * \brief Lance un thread avec la fonction run()
     */
    void startThread() {
        if (thread == nullptr) {
            printStartMessage();
            thread = std::make_unique<PcoThread>(&Launchable::run, this);
        }
    }

    /*!
     * \brief Attend la fin du thread lancé
     */
    void join() {
        if (thread != nullptr) {
            thread->join();
            thread = nullptr;
            printCompletionMessage();
        }
    };

protected:

    /*!
     * \brief La fonction à lancer, ici abstraite (virtuelle pure), est redéfinie par les
     * classes concrètes qui héritent de la classe Launchable.
     */
    virtual void run() = 0;

    /*!
     * \brief Message affiché au lancement du thread
     */
    virtual void printStartMessage() const {qDebug() << "[START] Un thread lancé";}

    /*!
     * \brief Message affiché après la fin du thread
     */
    virtual void printCompletionMessage() const {qDebug() << "[STOP] Un thread a terminé";}

    /*!
     * \brief Le thread associé
     */
    std::unique_ptr<PcoThread> thread = nullptr;


};

#endif // LAUNCHABLE_H
