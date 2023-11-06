#ifndef WINDOWINTERFACE_H
#define WINDOWINTERFACE_H

#include <QObject>
#include <iostream>
#include <QMessageBox>
#include "mainwindow.h"
#include "seller.h"

class Utils;

class WindowInterface : public QObject
{
    Q_OBJECT

public:
    WindowInterface();

    virtual ~WindowInterface(){}

    static void initialize(unsigned int nbExtractors, unsigned int nbFactories, unsigned int nbWholesalers);

    void consoleAppendText(unsigned int consoleId, QString text);

    void updateFund(unsigned int id, unsigned new_fund);
    void updateStock(unsigned int id, std::map<ItemType, int>* stocks);
    void setLink(int from, int to);
    void setUtils(Utils* utils);

private:
    static bool sm_didInitialize;
    static MainWindow *mainwindow;


signals:
    void sig_consoleAppendText(unsigned int consoleId, QString text);

    void sig_updateFund(unsigned int id, unsigned new_fund);
    void sig_updateStock(unsigned int id, std::map<ItemType, int>* stocks);
    void sig_set_link(int from, int to);
};

#endif // WINDOWINTERFACE_H
