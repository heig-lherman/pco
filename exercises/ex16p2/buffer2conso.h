#ifndef BUFFER2CONSO_H
#define BUFFER2CONSO_H

#include "abstractbuffer.h"
#include <pcosynchro/pcoconditionvariable.h>
#include <pcosynchro/pcomutex.h>
#include <vector>

template <typename T> class Buffer2ConsoMesa : public AbstractBuffer<T> {
protected:
  const unsigned int CONSUMERS = 2;

  PcoMutex mutex;
  PcoConditionVariable notEmpty, notFull;

  std::vector<T> elements;
  int head{0}, tail{0};
  unsigned nbElements{0}, nbConsumed{0}, bufferSize;

public:
  Buffer2ConsoMesa(unsigned int bufferSize)
      : elements(bufferSize), bufferSize(bufferSize) {}

  virtual ~Buffer2ConsoMesa() {}

  virtual void put(T item) {
    mutex.lock();

    while (nbElements == bufferSize) {
      notFull.wait(&mutex);
    }

    elements[head] = item;
    head = (head + 1) % bufferSize;
    nbElements++;

    for (unsigned i = 0; i < CONSUMERS; ++i) {
      notEmpty.notifyOne();
    }

    mutex.unlock();
  }

  virtual T get(void) {
    T item;
    mutex.lock();

    while (nbElements == 0) {
      notEmpty.wait(&mutex);
    }

    item = elements[tail];
    nbConsumed++;

    if (nbConsumed == CONSUMERS) {
      nbConsumed = 0;
      tail = (tail + 1) % bufferSize;
      nbElements--;

      notFull.notifyOne();
    }

    mutex.unlock();
    return item;
  }
};

#endif // BUFFER2CONSO_H
