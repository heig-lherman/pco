#ifndef COMPUTEENVIRONMENTGUI_H
#define COMPUTEENVIRONMENTGUI_H

#include "computeenvironment.h"
#include "computeenginegui.h"

class ComputeEnvironmentGui : public ComputeEnvironment
{
public:
    ComputeEnvironmentGui(std::shared_ptr<ComputationManager> computationManager): ComputeEnvironment(computationManager) {}

    void addComputeEngine(ComputationType type, unsigned int quantity = 1) override {
        for (unsigned i = 0; i < quantity; ++i) {
            switch(type) {
            case ComputationType::A :
                threads.push_back(std::make_shared<ComputeEngineGUI>(std::make_unique<ComputeEngineA>(computationManager)));
                break;
            case ComputationType::B :
                 threads.push_back(std::make_shared<ComputeEngineGUI>(std::make_unique<ComputeEngineB>(computationManager)));
                break;
            case ComputationType::C :
                 threads.push_back(std::make_shared<ComputeEngineGUI>(std::make_unique<ComputeEngineC>(computationManager)));
                break;
            default:
                break;
            }
        }
    }
};

#endif // COMPUTEENVIRONMENTGUI_H
