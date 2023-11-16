#ifndef CONNECT_H
#define CONNECT_H


#include <QMessageBox>
#include <QObject>

/*
  Copyright : Yann Thoma, REDS, HEIG-VD.
*/

/*! It simply connects a signal to a slot and opens a message box if something
  goes wrong. Very useful for debugging, and could be simplified for a release
*/
#define CONNECT(a,b,c,d) \
        if (!connect(a,b,c,d))\
                        {\
                                QString mess;\
                                mess =QString("Signal connection error in file %1, line %2. %3: %4::%5").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__).arg(#a).arg(#b);\
                                QMessageBox::warning(nullptr, "QTrainSim",mess);\
                        }


#endif // CONNECT_H
