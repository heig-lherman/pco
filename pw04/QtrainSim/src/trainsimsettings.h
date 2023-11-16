#ifndef TRAINSIMSETTINGS_H
#define TRAINSIMSETTINGS_H

class TrainSimSettings
{

public:

    static TrainSimSettings *getInstance();

    bool getViewContactNumber();
    void setViewContactNumber(bool draw);

    bool getViewAiguillageNumber();
    void setViewAiguillageNumber(bool draw);

    bool getViewLocoLog();
    void setViewLocoLog(bool view);

    bool getInertie();
    void setInertie(bool enable);

protected:
    TrainSimSettings();

    bool viewContactNumber;
    bool viewAiguillageNumber;
    bool viewLocoLog;
    bool inertie;
};


#endif // TRAINSIMSETTINGS_H
