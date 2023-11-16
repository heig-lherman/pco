#include "trainsimsettings.h"

TrainSimSettings::TrainSimSettings()
{
    viewLocoLog = false;
    viewContactNumber = false;
    viewAiguillageNumber = false;
    inertie = true;
}


TrainSimSettings* ::TrainSimSettings::getInstance()
{
    static TrainSimSettings instance;
    return &instance;
}

bool TrainSimSettings::getViewLocoLog()
{
    return viewLocoLog;
}

void TrainSimSettings::setViewLocoLog(bool view)
{
    viewLocoLog = view;
}

bool TrainSimSettings::getViewAiguillageNumber()
{
    return viewAiguillageNumber;
}

void TrainSimSettings::setViewAiguillageNumber(bool draw)
{
    viewAiguillageNumber = draw;
}

bool TrainSimSettings::getViewContactNumber()
{
    return viewContactNumber;
}

void TrainSimSettings::setViewContactNumber(bool draw)
{
    viewContactNumber = draw;
}

bool TrainSimSettings::getInertie()
{
    return inertie;
}

void TrainSimSettings::setInertie(bool enable)
{
    inertie = enable;
}

