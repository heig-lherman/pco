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

#include <pcosynchro/pcoconditionvariable.h>

#include "abstractreaderwriter.h"

class ReaderWriterClassAB {
protected:
  PcoMutex mutex;
  PcoConditionVariable alock, block;
  int acount{0}, bcount{0};

public:
  ReaderWriterClassAB() {}

  void lockA() {
    mutex.lock();

    while (bcount > 0) {
      alock.wait(&mutex);
    }

    acount++;

    mutex.unlock();
  }

  void unlockA() {
    mutex.lock();

    acount--;
    if (acount == 0) {
      block.notifyAll();
    }

    mutex.unlock();
  }

  void lockB() {
    mutex.lock();

    while (acount > 0) {
      block.wait(&mutex);
    }

    bcount++;

    mutex.unlock();
  }

  void unlockB() {
    mutex.lock();

    bcount--;
    if (bcount == 0) {
      alock.notifyAll();
    }

    mutex.unlock();
  }
};
#endif // READERWRITERCLASSAB_H
