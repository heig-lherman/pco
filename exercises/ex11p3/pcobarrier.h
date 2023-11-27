#ifndef PCOBARRIER_H
#define PCOBARRIER_H

#include <pcosynchro/pcohoaremonitor.h>

class PcoBarrier : public PcoHoareMonitor {
  int nbWaiting;
  int total;
  PcoHoareMonitor::Condition cond;

public:
  PcoBarrier(unsigned int nbToWait) : nbWaiting(0), total(nbToWait) {}

  ~PcoBarrier() {}

  void arrive() {
    monitorIn();
    nbWaiting++;
    if (nbWaiting < total) {
      wait(cond);
      signal(cond);
    } else {
      nbWaiting = 0;
      signal(cond);
    }
    monitorOut();
  }
};

#endif // PCOBARRIER_H
