#include <QVector>
#include <pcosynchro/pcothread.h>

#include "threadmanager.h"
#include "mythread.h"

/*
 * std::pow pour les long long unsigned int
 */
long long unsigned int intPow(
        long long unsigned int number,
        long long unsigned int index
) {
    long long unsigned int i;

    if (index == 0)
        return 1;

    long long unsigned int num = number;

    for (i = 1; i < index; i++)
        number *= num;

    return number;
}

/**
 * Function to evenly split the amount of computation to do between threads
 *
 * @param nbToCompute number of computation to do
 * @param nbThreads   number of threads
 * @param threadsComputeAmount vector to fill with the amount of computation to
 */
void splitThreadsComputeAmount(
        unsigned long long nbToCompute,
        unsigned int nbThreads,
        QVector<unsigned long long> &threadsComputeAmount
) {
    if (nbToCompute % nbThreads == 0) {
        threadsComputeAmount.fill(nbToCompute / nbThreads);
        return;
    }

    unsigned long long remainder = nbThreads - (nbToCompute % nbThreads);
    unsigned long long meanValue = nbToCompute / nbThreads;
    for (unsigned i = 0; i < nbThreads; ++i) {
        if (i >= remainder) {
            threadsComputeAmount[i] = meanValue + 1;
        } else {
            threadsComputeAmount[i] = meanValue;
        }
    }
}

ThreadManager::ThreadManager(QObject *parent) : QObject(parent) {}

void ThreadManager::incrementPercentComputed(double percentComputed) {
    emit sig_incrementPercentComputed(percentComputed);
}

/*
 * Les paramètres sont les suivants:
 *
 * - charset:   QString contenant tous les caractères possibles du mot de passe
 * - salt:      le sel à concaténer au début du mot de passe avant de le hasher
 * - hash:      le hash dont on doit retrouver la préimage
 * - nbChars:   le nombre de caractères du mot de passe à bruteforcer
 * - nbThreads: le nombre de threads à lancer
 *
 * Cette fonction doit retourner le mot de passe correspondant au hash, ou une
 * chaine vide si non trouvé.
 */
QString ThreadManager::startHacking(
        QString charset,
        QString salt,
        QString hash,
        unsigned int nbChars,
        unsigned int nbThreads
) {
    // Calcul du nombre de hash à générer
    unsigned long long nbToCompute = intPow(charset.length(), nbChars);

    // Vecteur de threads et calcul du nombre à calculer par thread
    QVector<PcoThread *> threads(nbThreads);
    QVector<unsigned long long> threadsComputeAmount(nbThreads);
    splitThreadsComputeAmount(nbToCompute, nbThreads, threadsComputeAmount);

    // index de départ pour chaque thread qui sera incrémenté
    unsigned long long startingIndex = 0;

    // variable pour le résultat et pour savoir si le résultat est trouvé
    QString result;

    // instantiate threads
    for (int i = 0; i < nbThreads; ++i) {
        threads[i] = new PcoThread(
                runComputation,
                charset,
                salt,
                hash,
                nbChars,
                threadsComputeAmount[i],
                startingIndex,
                [&, this](auto &&count) {
                    incrementPercentComputed(count / (double) nbToCompute);
                },
                &result
        );
        startingIndex += threadsComputeAmount[i];
    }

    // Attente des threads et nettoyage
    for (const auto &thread: threads) {
        thread->join();
        delete thread;
    }

    return result;
}
