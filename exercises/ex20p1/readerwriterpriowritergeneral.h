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
#include <pcosynchro/pcosemaphore.h>

#include "abstractreaderwriter.h"

class ReaderWriterPrioWriterGeneral : public AbstractReaderWriter {
protected:
  PcoSemaphore rmutex{1}, wmutex{1}, readTry{1}, resource{1};
  int rcount{0}, wcount{0};

public:
  ReaderWriterPrioWriterGeneral() {}

  void lockReading() {
    readTry.acquire();
    rmutex.acquire();

    rcount++;
    if (rcount == 1)
      resource.acquire();

    rmutex.release();
    readTry.release();
  }

  void unlockReading() {
    rmutex.acquire();

    rcount--;
    if (rcount == 0)
      resource.release();

    rmutex.release();
  }

  void lockWriting() {
    wmutex.acquire();

    wcount++;
    if (wcount == 1)
      readTry.acquire();

    wmutex.release();
    resource.acquire();
  }

  void unlockWriting() {
    wmutex.acquire();

    wcount--;
    if (wcount == 0)
      readTry.release();

    wmutex.release();
  }
};

#endif // READERWRITERPRIOWRITERGENERAL_H
