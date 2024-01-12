#ifndef TESTCOMPUTENGINE_H
#define TESTCOMPUTENGINE_H

#include "computeengine.h"

class TestComputeEngine : public ComputeEngineCommon
{
public:
    TestComputeEngine(std::shared_ptr<ComputationManager> computationManager, ComputationType type, unsigned steps, unsigned delay): AbstractComputeEngine(computationManager, 0), type(type), steps(steps), delay(delay) {}

protected:
    ComputationType myType() const override {return type;}

    void startComputation(const Request& r) override {
        ComputeEngineCommon::startComputation(r);
        computationDone = false;
        result = getCurrentRequestId();
        stepCounter = steps;
    }

    void advanceComputation() override {
        if (stepCounter-- > 0) {
            // delay;
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        } else {
            computationDone = true;
        }
    }

    void printStartMessage() const override {}
    void printCompletionMessage() const override {}
private:
    const ComputationType type;
    const unsigned steps;
    const unsigned delay;
    unsigned stepCounter;
};

#endif // TESTCOMPUTENGINE_H
