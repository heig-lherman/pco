#ifndef BUFFER2CONSO_H
#define BUFFER2CONSO_H

#include "abstractbuffer.h"
#include <pcosynchro/pcosemaphore.h>

template <typename T> class Buffer2ConsoSemaphore : public AbstractBuffer<T> {
protected:
  const int CONSUMERS = 2;

  std::vector<T> elements;
  int writePointer{0}, readPointer{0}, nbElements{0}, nbConsumed{0};
  PcoSemaphore mutex{1}, waitProd{0}, waitConso{0};
  unsigned nbWaitingProd{0}, nbWaitingConso{0}, bufferSize;

public:
  Buffer2ConsoSemaphore(unsigned bufferSize)
      : elements(bufferSize), bufferSize(bufferSize) {}

  virtual ~Buffer2ConsoSemaphore() {}

  virtual void put(T item) {
    mutex.acquire();

    if (nbElements == bufferSize) {
      nbWaitingProd++;
      mutex.release();
      waitProd.acquire();
    }

    elements[writePointer] = item;
    writePointer = (writePointer + 1) % bufferSize;
    nbElements++;

    if (nbWaitingConso > 0) {
      nbWaitingConso--;
      waitConso.release();
    } else {
      mutex.release();
    }
  }

  virtual T get(void) {
    T item;
    mutex.acquire();

    if (nbElements == 0) {
      nbWaitingConso++;
      mutex.release();
      waitConso.acquire();
    }

    item = elements[readPointer];
    ++nbConsumed;

    if (nbConsumed == CONSUMERS) {
      nbConsumed = 0;
      readPointer = (readPointer + 1) % bufferSize;
      nbElements--;

      if (nbWaitingProd > 0) {
        nbWaitingProd--;
        waitProd.release();
      } else {
        mutex.release();
      }

      return item;
    }

    if (nbWaitingConso > 0) {
      --nbWaitingConso;
      waitConso.release();
    } else {
      mutex.release();
    }

    return item;
  }
};

#endif // BUFFER2CONSO_H
