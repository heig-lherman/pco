#ifndef BUFFER2CONSO_H
#define BUFFER2CONSO_H

#include "abstractbuffer.h"
#include <pcosynchro/pcohoaremonitor.h>

template <typename T>
class Buffer2ConsoHoare : public AbstractBuffer<T>, public PcoHoareMonitor {
protected:
  const unsigned int CONSUMERS = 2;

  PcoHoareMonitor::Condition notFull, notEmpty;

  std::vector<T> elements;
  int head{0}, tail{0};
  unsigned nbElements{0}, nbConsumed{0}, bufferSize;

public:
  Buffer2ConsoHoare(unsigned int bufferSize)
      : elements(bufferSize), bufferSize(bufferSize) {}

  virtual ~Buffer2ConsoHoare() {}

  virtual void put(T item) {
    monitorIn();

    if (nbElements == bufferSize) {
      wait(notFull);
    }

    elements[head] = item;
    head = (head + 1) % bufferSize;
    nbElements++;

    for (unsigned int i = 0; i < CONSUMERS; i++) {
      signal(notEmpty);
    }

    monitorOut();
  }

  virtual T get(void) {
    T item;
    monitorIn();

    if (nbElements == 0) {
      wait(notEmpty);
    }

    item = elements[tail];
    nbConsumed++;

    if (nbConsumed == CONSUMERS) {
      nbConsumed = 0;
      tail = (tail + 1) % bufferSize;
      nbElements--;

      signal(notFull);
    }

    monitorOut();
    return item;
  }
};

#endif // BUFFER2CONSO_H
