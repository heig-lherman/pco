#include "computeenginegui.h"
#include "guiinterface.h"

int ComputeEngineGUI::nextGuiId = 0;

ComputeEngineGUI::ComputeEngineGUI(std::unique_ptr<ComputeEngineCommon> ce) : AbstractComputeEngine(ce->computationManager, ce->id), computeEngine(std::move(ce)), guiId(nextGuiId++) {
    myName = "Compute Engine " + nameFromType(myType()) + "-" + std::to_string(computeEngine->id);
    //std::cout << myName << " will register" << std::endl;
    GuiInterface::instance->registerComputeEngine(guiId, myName.c_str());
}

void ComputeEngineGUI::startComputation(const Request& r) {
    //qDebug() << guiId << "GUI start comp";
    auto t = GuiInterface::instance->getCurrentTime();
    //qDebug() << "time :" << t;
    GuiInterface::instance->addTaskStart(guiId, t);
    computeEngine->startComputation(r);
    lastTime = t;
    started = true;
}

void ComputeEngineGUI::advanceComputation() {
    // Add a delay
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    computeEngine->advanceComputation();
    auto t = GuiInterface::instance->getCurrentTime();
    GuiInterface::instance->addTaskExecute(guiId, lastTime, t);
    lastTime = t;
}

void ComputeEngineGUI::stopComputation() {
    if (started) {
        //qDebug() << guiId << "GUI stop comp";
        auto t = GuiInterface::instance->getCurrentTime();
        //qDebug() << "time :" << t;
        GuiInterface::instance->addTaskExecute(guiId, lastTime, t);
        GuiInterface::instance->addTaskEnd(guiId, t);
        computeEngine->stopComputation();
        started = false;
    }
}
