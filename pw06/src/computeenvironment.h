//     ____  __________     ___   ____ ___  _____ //
//    / __ \/ ____/ __ \   |__ \ / __ \__ \|__  / //
//   / /_/ / /   / / / /   __/ // / / /_/ / /_ <  //
//  / ____/ /___/ /_/ /   / __// /_/ / __/___/ /  //
// /_/    \____/\____/   /____/\____/____/____/   //
// Auteur : Rick Wertenbroek
//
// Modifications : (si vous effectuez des changements notez le ici)
// Auteurs : Prénom Nom, Prénom Nom

#ifndef COMPUTEENVIRONMENT_H
#define COMPUTEENVIRONMENT_H

#include "computationmanager.h"
#include "computeengine.h"

/**
 * @brief The ComputeEnvironment class represents a compute environment with compute engines and allows to launch them
 */
class ComputeEnvironment
{
public:
    /**
     * @brief ComputeEnvironment Constructs the compute environment that is attached to a given buffer
     * @param computationManager
     */
    ComputeEnvironment(std::shared_ptr<ComputationManager> computationManager): computationManager(std::move(computationManager)) {}

    /**
     * @brief populateComputeEnvironment adds compute engines to the environment
     */
    void populateComputeEnvironment() {
        addComputeEngine(ComputationType::A, 2);
        addComputeEngine(ComputationType::B);
        addComputeEngine(ComputationType::C);
    }

    /**
     * @brief startComputeEnvironment starts the compute engines in the environment
     */
    void startComputeEnvironment() {
        for (auto& t : threads) {
            t->startThread();
        }
    }

    /**
     * @brief joinComputeEnvironment joins the compute engines in the environment
     */
    void joinComputeEnvironment() {
        for (auto& t : threads) {
            t->join();
        }
    }

protected:
    /**
     * @brief addComputeEngine Helper function to add compute engines
     * @param type
     * @param quantity
     */
    virtual void addComputeEngine(ComputationType type, unsigned quantity = 1) {
        for (unsigned i = 0; i < quantity; ++i) {
            switch(type) {
            case ComputationType::A :
                threads.push_back(std::make_shared<ComputeEngineA>(computationManager));
                break;
            case ComputationType::B :
                threads.push_back(std::make_shared<ComputeEngineB>(computationManager));
                break;
            case ComputationType::C :
                threads.push_back(std::make_shared<ComputeEngineC>(computationManager));
                break;
            default:
                break;
            }
        }
    }

    std::vector<std::shared_ptr<Launchable>> threads;
    std::shared_ptr<ComputationManager> computationManager;
};

#endif // COMPUTEENVIRONMENT_H
