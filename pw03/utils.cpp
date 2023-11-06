#include "utils.h"


void Utils::endService() {
    std::cout << "It's time to end !" << std::endl;
    for(auto& thread : threads) {
        thread->requestStop();
    }
}

void Utils::externalEndService() {
    endService();
    semEnd.acquire();
    utilsThread->join();
}

std::vector<Extractor*> createExtractors(int nbExtractors, int idStart) {
    if (nbExtractors < 1){
        qInfo() << "Cannot make the programm work with less than 1 extractor";
        exit(-1);
    }

    std::vector<Extractor*> extractors;

    for(int i = 0; i < nbExtractors; ++i) {
        switch(i % 3) {
            case 0:
                extractors.push_back(new SandExtractor(i + idStart, EXTRACTOR_FUND));
                break;

            case 1:
                extractors.push_back(new CopperExtractor(i + idStart, EXTRACTOR_FUND));
                break;

            case 2:
                extractors.push_back(new PetrolExtractor(i + idStart, EXTRACTOR_FUND));
                break;
        }
    }


    return extractors;
}

std::vector<Factory*> createFactories(int nbFactories, int idStart) {
    if (nbFactories < 1){
        qInfo() << "Cannot make the programm work with less than 1 Factory";
        exit(-1);
    }

    std::vector<Factory*> factories;

    for(int i = 0; i < nbFactories; ++i) {
        switch(i % 3) {
            case 0:
                factories.push_back(new PlasticFactory(i + idStart, FACTORIES_FUND));
                break;

            case 1:
                factories.push_back(new ChipFactory(i + idStart, FACTORIES_FUND));
                break;

            case 2:
                factories.push_back(new RobotFactory(i + idStart, FACTORIES_FUND));
                break;
        }
    }


    return factories;
}

std::vector<Wholesale*> createWholesaler(int nbWholesaler, int idStart) {
    if(nbWholesaler < 1){
        qInfo() << "Cannot launch the programm without any wholesaler";
        exit(-1);
    }

    std::vector<Wholesale*> wholesalers;

    for(int i = 0; i < nbWholesaler; ++i){
        wholesalers.push_back(new Wholesale(i + idStart, WHOLESALERS_FUND));
    }

    return wholesalers;
}


Utils::Utils(int nbExtractor, int nbFactory, int nbWholesale) {
    this->extractors.resize(nbExtractor);
    this->wholesalers.resize(nbWholesale);
    this->factories.resize(nbFactory);

    this->extractors = createExtractors(nbExtractor, 0);
    this->wholesalers = createWholesaler(nbWholesale, nbExtractor);
    this->factories = createFactories(nbFactory, nbExtractor + nbWholesale);

    for(auto i = factories.begin(); i != factories.end(); ++i) {
        (*i)->setWholesalers(wholesalers);
    }

    int extractorsByWholesaler = nbExtractor / nbWholesale;
    int extractorsShared = nbExtractor % nbWholesale;

    int factoriesByWholesaler = nbFactory / nbWholesale;
    int factoriesShared = nbFactory % nbWholesale;

    int countExtractor = 0;
    int countFactory = 0;

    for(auto& w : wholesalers) {

        std::vector<Extractor*> tmpExtractors(extractors.begin() + countExtractor, extractors.begin() + countExtractor + extractorsByWholesaler);
        tmpExtractors.insert(tmpExtractors.end(), extractors.end() - extractorsShared, extractors.end());

        std::vector<Factory*> tmpFactories(factories.begin() + countFactory, factories.begin() + countFactory + factoriesByWholesaler);
        tmpFactories.insert(tmpFactories.end(), factories.end() - factoriesShared, factories.end());

        countExtractor += extractorsByWholesaler;
        countFactory += factoriesByWholesaler;

        std::vector<Seller*> sellers;
        for (auto& m : tmpExtractors)
            sellers.push_back(static_cast<Seller*>(m));
        for (auto& f : tmpFactories)
            sellers.push_back(static_cast<Seller*>(f));
        w->setSellers(sellers);
    }

    utilsThread = std::make_unique<PcoThread>(&Utils::run, this);
}

void Utils::run() {
    for(size_t i = 0; i < extractors.size(); ++i) {
        threads.emplace_back(std::make_unique<PcoThread>(&Extractor::run, extractors[i]));
    }
    for(size_t i = 0; i < factories.size(); ++i) {
        threads.emplace_back(std::make_unique<PcoThread>(&Factory::run, factories[i]));
    }
    for(size_t i = 0; i < wholesalers.size(); ++i) {
        threads.emplace_back(std::make_unique<PcoThread>(&Wholesale::run, wholesalers[i]));
    }

    for (auto& thread : threads) {
        thread->join();
    }

    int startFund = (EXTRACTOR_FUND * int(extractors.size()) + (FACTORIES_FUND * int(factories.size()) + (WHOLESALERS_FUND * int(wholesalers.size()))));
    int endFund = 0;

    for(Extractor* extractor: extractors) {
        endFund += extractor->getFund();
        endFund += extractor->getAmountPaidToMiners();
    }

    for(Factory* factory: factories) {
        endFund += factory->getFund();
        endFund += factory->getAmountPaidToWorkers();
    }

    for(Wholesale* wholesale : wholesalers) {
        endFund += wholesale->getFund();
    }

    finalReport = QString("The expected fund is : %1 and you got at the end : %2").arg(startFund).arg(endFund);

    qInfo() << "The expected fund is : " << startFund << " and you got at the end : " << endFund;
    semEnd.release();
}

QString Utils::getFinalReport()
{
    return finalReport;
}
