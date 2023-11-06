#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <QRandomGenerator>
#include <iostream>
#include <pcosynchro/pcothread.h>
#include <QDebug>
#include <QTextStream>

#include "extractor.h"
#include "factory.h"
#include "wholesale.h"
#include "seller.h"

#define NB_EXTRACTOR 3
#define NB_FACTORIES 3
#define NB_WHOLESALER 2
#define EXTRACTOR_FUND 200
#define FACTORIES_FUND 300
#define WHOLESALERS_FUND 250

std::vector<Extractor*> createExtractors(int nbExtractors, int idStart);
std::vector<Factory*> createFactories(int nbFactories, int idStart);
std::vector<Wholesale*> createWholesaler(int nbWholesaler, int idStart);

class Utils {
public:
    void externalEndService();
    QString getFinalReport();

private:
    std::vector<Extractor*> extractors;
    std::vector<Factory*> factories;
    std::vector<Wholesale*> wholesalers;

    std::vector<std::unique_ptr<PcoThread>> threads;
    std::unique_ptr<PcoThread> utilsThread;

    QString finalReport;

    void endService();

    void run();

    PcoSemaphore semEnd{0};
public:
    Utils(int nbExtractor, int nbFactory, int nbWholesale);


};

#endif // UTILS_H
