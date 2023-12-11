/** @file readerwritepriowritergeneral.h
 *  @brief Reader-writer with priority for readers
 *
 *  Implementation of a reader-writer resource manager with priority
 *  for readers. This class follows the "general" way of solving
 *  concurrent problems: internal variables, one mutex, and semaphores
 *  as synchronization primitives.
 *
 *
 *  @author Yann Thoma
 *  @date 08.05.2017
 *  @bug No known bug
 */

#ifndef READERWRITERPRIOWRITERGENERAL_H
#define READERWRITERPRIOWRITERGENERAL_H

#include <pcosynchro/pcoconditionvariable.h>

#include "abstractreaderwriter.h"

class ReaderWriterPrioWriterGeneral : public AbstractReaderWriter {
protected:
  PcoMutex mutex;
  PcoConditionVariable lock;
  int rcount{0}, wcount{0};
  bool wactive{false};

public:
  ReaderWriterPrioWriterGeneral() {}

  void lockReading() {
    mutex.lock();

    while (wcount > 0 || wactive) {
      lock.wait(&mutex);
    }

    rcount++;

    mutex.unlock();
  }

  void unlockReading() {
    mutex.lock();

    rcount--;
    if (rcount == 0) {
      lock.notifyAll();
    }

    mutex.unlock();
  }

  void lockWriting() {
    mutex.lock();

    wcount++;
    while (rcount > 0 || wactive) {
      lock.wait(&mutex);
    }

    wcount--;
    wactive = true;

    mutex.unlock();
  }

  void unlockWriting() {
    mutex.lock();

    wactive = false;
    lock.notifyAll();

    mutex.unlock();
  }
};

#endif // READERWRITERPRIOWRITERGENERAL_H
