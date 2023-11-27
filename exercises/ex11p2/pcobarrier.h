#ifndef PCOBARRIER_H
#define PCOBARRIER_H

#include <pcosynchro/pcoconditionvariable.h>
#include <pcosynchro/pcomutex.h>

class PcoBarrier {
  PcoMutex mutex;
  PcoConditionVariable cond;
  int nbWaiting;
  int total;

public:
  PcoBarrier(unsigned int nbToWait) : nbWaiting(0), total(nbToWait) {}

  ~PcoBarrier() {}

  void wait() {
    mutex.lock();
    if (nbWaiting == total - 1) {
      nbWaiting = 0;
      cond.notifyAll();
    } else {
      nbWaiting++;
      cond.wait(&mutex);
    }
    mutex.unlock();
  }
};

#endif // PCOBARRIER_H
