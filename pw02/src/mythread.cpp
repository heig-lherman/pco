#include "mythread.h"

#include <QCryptographicHash>
#include <QVector>

/**
 * Function to build the string converting the index as base to the
 * charset's base
 * @param charset charset to use
 * @param index   the index for the current string
 * @param result the string to fill
 */
void buildString(
        const QString &charset,
        unsigned long long index,
        QString &result
) {
    unsigned nbValidChars = charset.length();
    unsigned long long nbChars = result.length();

    // On traduit l'index en base nbValidChars
    for (unsigned i = 0; i < nbChars; ++i) {
        result[i] = charset.at((int) (index % nbValidChars));
        index /= nbValidChars;
    }
}

void runComputation(
        const QString &charset,
        const QString &salt,
        const QString &hash,
        unsigned nbChars,
        unsigned long long nbToCompute,
        unsigned long long startingIndex,
        const std::function<void(unsigned)> &updateIterationCount,
        QString *result
) {
    // Nombre de caractères différents pouvant composer le mot de passe
    unsigned nbValidChars = charset.length();
    unsigned long long nbComputed = 0;

    // Mot de passe à tester courant
    QString currentPasswordString;
    // Hash du mot de passe à tester courant
    QString currentHash;
    // Object QCryptographicHash servant à générer des md5
    QCryptographicHash md5(QCryptographicHash::Md5);

    // On initialise le premier mot de passe à tester courant en le remplissant
    // de nbChars fois du premier caractère de charset
    currentPasswordString.fill(charset.at(0), nbChars);

    // on doit incrémenter le mot de passe testé de startingIndex fois
    if (startingIndex != 0) {
        buildString(charset, startingIndex, currentPasswordString);
    }

    while (nbComputed < nbToCompute && result->isEmpty()) {
        // On vide les données déjà ajoutées au générateur
        md5.reset();
        // On préfixe le mot de passe avec le sel
        md5.addData(salt.toLatin1());
        md5.addData(currentPasswordString.toLatin1());
        // On calcul le hash
        currentHash = md5.result().toHex();

        // Si on a trouvé, on retourne le mot de passe courant (sans le sel)
        if (currentHash == hash) {
            *result = currentPasswordString;
            return;
        }

        // Tous les 1000 hash calculés, on notifie qui veut bien entendre
        // de l'état de notre avancement (pour la barre de progression)
        if ((nbComputed % 1000) == 0) {
            updateIterationCount(1000);
        }

        // On incrémente le mot de passe courant
        buildString(charset, ++startingIndex, currentPasswordString);

        ++nbComputed;
    }
}
