/** @file readerwriterclassab.h
 *  @brief Reader-writer with equal priority between two classes
 *
 *  Implementation of a reader-writer resource manager with equal
 *  priority for both classes. Mutual exclusion between classes must be
 *  ensured
 *
 *
 *
 *  @author Yann Thoma
 *  @date 15.05.2017
 *  @bug No known bug
 */

#ifndef READERWRITERCLASSAB_H
#define READERWRITERCLASSAB_H

#include <pcosynchro/pcohoaremonitor.h>

#include "abstractreaderwriter.h"

class ReaderWriterClassAB : public PcoHoareMonitor {
protected:
  PcoHoareMonitor::Condition alock, block;
  int acount{0}, bcount{0}, awcount{0}, bwcount{0};

public:
  ReaderWriterClassAB() {}

  void lockA() {
    monitorIn();

    if (bcount > 0) {
      awcount++;
      wait(alock);
      awcount--;
    }

    acount++;
    if (awcount > 0) {
      signal(alock);
    }

    monitorOut();
  }

  void unlockA() {
    monitorIn();

    acount--;
    if (acount == 0 && bwcount > 0) {
      signal(block);
    }

    monitorOut();
  }

  void lockB() {
    monitorIn();

    if (acount > 0) {
      bwcount++;
      wait(block);
      bwcount--;
    }

    bcount++;
    if (bwcount > 0) {
      signal(block);
    }

    monitorOut();
  }

  void unlockB() {
    monitorIn();

    bcount--;
    if (bcount == 0 && awcount > 0) {
      signal(alock);
    }

    monitorOut();
  }
};
#endif // READERWRITERCLASSAB_H
