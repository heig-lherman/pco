#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <pcosynchro/pcothread.h>
#include <QString>

void runComputation(
        const QString &charset,
        const QString &salt,
        const QString &hash,
        unsigned nbChars,
        unsigned long long nbToCompute,
        unsigned long long startingIndex,
        const std::function<void(unsigned)> &updateIterationCount,
        QString *result
);

#endif // MYTHREAD_H
