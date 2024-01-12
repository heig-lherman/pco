#ifndef COMPUTEENGINEGUI_H
#define COMPUTEENGINEGUI_H

#include "computeengine.h"

// Decorator
/* Note : This decorator only redefines methods used by run() and not by the wrapped compute engine
 * This makes it impossible to instrumentalize functions used by the compute engine... */
class ComputeEngineGUI : public virtual AbstractComputeEngine, public ComputeEngineBehavior
{
public:
    ComputeEngineGUI(std::unique_ptr<ComputeEngineCommon> ce);
protected:

    // Compute Engine Specific Functions
    ComputationType myType() const override {return computeEngine->myType();}
    void startComputation(const Request& r) override;
    void advanceComputation() override;
    bool isComputationDone() const override {return computeEngine->isComputationDone();}
    double getResult() const override {return computeEngine->result;}
    int getCurrentRequestId() const override {return computeEngine->currentRequest.getId();}
    void stopComputation() override;

    // Behavior function
    //void run() override {computeEngine->run();} // WARNING : Do not use the compute engine run but our own
    void printStartMessage() const override {qDebug() << "GUI Enabled :" << guiId; computeEngine->printStartMessage();}
    void printCompletionMessage() const override {qDebug() << "GUI Enabled :" << guiId; computeEngine->printCompletionMessage();}

    std::unique_ptr<ComputeEngineCommon> computeEngine;
private:
    // I would love some reflection here ...
    std::string nameFromType(ComputationType ct) {
        // This is inefficient but called only once
        switch (ct) {
        case ComputationType::A : return std::string("A");
        case ComputationType::B : return std::string("B");
        case ComputationType::C : return std::string("C");
        default: return std::string("?");
        }
    }

    std::string myName;
    long long lastTime;
    int guiId;
    bool started = false;
    static int nextGuiId;
};

#endif // COMPUTEENGINEGUI_H
