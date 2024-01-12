//     ____  __________     ___   ____ ___  _____ //
//    / __ \/ ____/ __ \   |__ \ / __ \__ \|__  / //
//   / /_/ / /   / / / /   __/ // / / /_/ / /_ <  //
//  / ____/ /___/ /_/ /   / __// /_/ / __/___/ /  //
// /_/    \____/\____/   /____/\____/____/____/   //
// Auteur : Rick Wertenbroek
// Rien à modifier ici

#ifndef COMPUTEENGINE_H
#define COMPUTEENGINE_H

#include <memory>
#include <cmath>
#include "computationmanager.h"
#include "launchable.h"

/**
 * @brief The AbstractComputeEngine class specifies the base functions that all compute engines
 * must at least have to describe their behavior.
 */
class AbstractComputeEngine
{
public:
    AbstractComputeEngine(std::shared_ptr<ComputeEngineInterface> computationManager, int id): computationManager(std::move(computationManager)), id(id) {}

protected:

    /**
     * @brief myType Returns the type of computation that the compute engine does
     * @return the computation type
     */
    [[nodiscard]] virtual ComputationType myType() const = 0;

    /**
     * @brief startComputation Starts a computation for a given request
     * @param r the computation request
     */
    virtual void startComputation(const Request& r) = 0;

    /**
     * @brief advanceComputation Advances the current computation
     */
    virtual void advanceComputation() = 0;

    /**
     * @brief isComputationDone Returns true if the computaion is done
     * @return true if the computation is done
     */
    [[nodiscard]] virtual bool isComputationDone() const = 0;

    /**
     * @brief getResult returns the result of the computation
     * @return the result
     */
    [[nodiscard]] virtual double getResult() const = 0;

    /**
     * @brief getCurrentRequestId Returns the id of the current request
     * @return the id of the current request
     */
    [[nodiscard]] virtual int getCurrentRequestId() const = 0;

    /**
     * @brief stopComputation Stops the current omputation
     */
    virtual void stopComputation() = 0;

    /**
     * @brief computationManager Pointer
     */
    const std::shared_ptr<ComputeEngineInterface> computationManager;

    /**
     * @brief id The compute engine id
     */
    const int id;
};

/**
 * @brief The ComputeEngineBehavior class describes the behavior of all compute engines
 * given the abstract class above. This allows to describe the behavior without any
 * knowledge of the specific internal implementations of specific compute engines.
 */
class ComputeEngineBehavior : private virtual AbstractComputeEngine, public Launchable
{
protected:

    /**
     * @brief run The behavior of a compute engine
     */
    void run() override {
        try {
            for(;;) {
                // Get a request from my type
                startComputation(computationManager->getWork(myType()));

                for(;;) {
                    // Continue with computation (do partial computation)
                    advanceComputation();

                    // If done provide the result to the manager
                    if (isComputationDone()) {
                        stopComputation();
                        computationManager->provideResult(Result(getCurrentRequestId(), getResult()));
                        break;
                    }
                    // else if I should not continue, stop
                    if (!computationManager->continueWork(getCurrentRequestId())) {
                        stopComputation();
                        break;
                    }
                }
            }
        // I got interrupted
        } catch (ComputationManager::StopException& e) {
            // Stop my computation
            stopComputation();
            return;
        }
    }
};

/**
 * @brief The ComputeEngineCommon class provides some common things for the specific engines below
 */
class ComputeEngineCommon : public virtual AbstractComputeEngine, public ComputeEngineBehavior
{
protected:
    Request currentRequest;
    std::shared_ptr<const std::vector<double>> data;
    bool computationDone = false;
    double result = 0.0;
    bool started = false;

    // Overriden functions, documentation is given in the AbstractComputeEngine class
    void startComputation(const Request& r) override {currentRequest = r; data = r.data; computationDone = false;}
    [[nodiscard]] bool isComputationDone() const override {return computationDone;}
    [[nodiscard]] double getResult() const override {return result;}
    [[nodiscard]] int getCurrentRequestId() const override {return currentRequest.getId();}
    void stopComputation() override {started = false;}

    // Allows the ComputeEngineGUI class to have access (to display events)
    friend class ComputeEngineGUI;
};

// The compute engines below are just examples and could do any type of operation

// Computation engine A will be an accumulator
class ComputeEngineA : public ComputeEngineCommon
{
public:
    ComputeEngineA(std::shared_ptr<ComputationManager> computationManager): AbstractComputeEngine(std::move(computationManager), nextId++) {}

protected:
    [[nodiscard]] ComputationType myType() const override {return ComputationType::A;}

    void startComputation(const Request& r) override {
        ComputeEngineCommon::startComputation(r);
        computationDone = false;
        started = true;
        result = 0.0;
        position = 0;
    }

    void advanceComputation() override {
        if (position < data->size()) {
            result += data->at(position++);
        } else {
            computationDone = true;
        }
    }

    void printStartMessage() const override {qDebug() << "[START] Compute Engine A -" << id << "launched";}
    void printCompletionMessage() const override {qDebug() << "[STOP] Compute Engine A -" << id;}
private:
    size_t position = 0;

    static int nextId;
};

// Computation engine B will be a multiplier
class ComputeEngineB : public ComputeEngineCommon
{
public:
    ComputeEngineB(std::shared_ptr<ComputationManager> computationManager): AbstractComputeEngine(std::move(computationManager), nextId++) {}

protected:
    [[nodiscard]] ComputationType myType() const override {return ComputationType::B;}

    void startComputation(const Request& r) override {
        ComputeEngineCommon::startComputation(r);
        computationDone = false;
        started = true;
        result = 1.0;
        position = 0;
    }

    void advanceComputation() override {
        if (position < data->size()) {
            result *= data->at(position++);
        } else {
            computationDone = true;
        }
    }

    void printStartMessage() const override {qDebug() << "[START] Compute Engine B -" << id << "launched";}
    void printCompletionMessage() const override {qDebug() << "[STOP] Compute Engine B -" << id;}
private:
    size_t position = 0;

    static int nextId;
};

// Computation engine C will be a simple divider
class ComputeEngineC : public ComputeEngineCommon
{
public:
    ComputeEngineC(std::shared_ptr<ComputationManager> computationManager): AbstractComputeEngine(std::move(computationManager), nextId++) {}
protected:
    [[nodiscard]] ComputationType myType() const override {return ComputationType::C;}

    void startComputation(const Request& r) override {
        ComputeEngineCommon::startComputation(r);
        computationDone = false;
        started = true;
    }

    void advanceComputation() override {
        if (data->size() != 2) {
            result = NAN;
            qDebug() << "Division requires exactly two operands";
        } else {
            result = data->at(0) / data->at(1);
        }
        computationDone = true;
    }

    void printStartMessage() const override {qDebug() << "[START] Compute Engine C -" << id << "launched";}
    void printCompletionMessage() const override {qDebug() << "[STOP] Compute Engine C -" << id;}
private:
    static int nextId;
};

#endif // COMPUTEENGINE_H
