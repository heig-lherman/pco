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

#include <pcosynchro/pcosemaphore.h>

#include "abstractreaderwriter.h"

class ReaderWriterClassAB {
protected:
  PcoSemaphore amutex{1}, bmutex{1}, resource{1};
  int acount{0}, bcount{0};

public:
  ReaderWriterClassAB() {}

  void lockA() {
    amutex.acquire();

    acount++;
    if (acount == 1)
      resource.acquire();

    amutex.release();
  }

  void unlockA() {
    amutex.acquire();

    acount--;
    if (acount == 0)
      resource.release();

    amutex.release();
  }

  void lockB() {
    bmutex.acquire();

    bcount++;
    if (bcount == 1)
      resource.acquire();

    bmutex.release();
  }

  void unlockB() {
    bmutex.acquire();

    bcount--;
    if (bcount == 0)
      resource.release();

    bmutex.release();
  }
};
#endif // READERWRITERCLASSAB_H
