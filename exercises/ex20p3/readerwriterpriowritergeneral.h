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

#include <pcosynchro/pcohoaremonitor.h>

#include "abstractreaderwriter.h"

class ReaderWriterPrioWriterGeneral : public AbstractReaderWriter,
                                      public PcoHoareMonitor {
protected:
  PcoHoareMonitor::Condition lock;

  int rcount{0}, wcount{0};
  bool wactive{false};

public:
  ReaderWriterPrioWriterGeneral() {}

  void lockReading() {
    monitorIn();

    if (wcount > 0 || wactive) {
      wait(lock);
    }

    rcount++;

    monitorOut();
  }

  void unlockReading() {
    monitorIn();

    rcount--;
    if (rcount == 0) {
      signal(lock);
    }

    monitorOut();
  }

  void lockWriting() {
    monitorIn();

    wcount++;
    if (rcount > 0 || wactive) {
      wait(lock);
    }

    wcount--;
    wactive = true;

    monitorOut();
  }

  void unlockWriting() {
    monitorIn();

    wactive = false;
    signal(lock);

    monitorOut();
  }
};

#endif // READERWRITERPRIOWRITERGENERAL_H
