#include <pcosynchro/pcohoaremonitor.h>

class BridgeManagerFloat : protected PcoHoareMonitor {
private:
  PcoHoareMonitor::Condition outbound;
  float maxWeight, curWeight{0};
  int nbWaiting{0};

public:
  BridgeManagerFloat(float maxWeight) : maxWeight(maxWeight) {}
  ~BridgeManagerFloat() {}

  void access(float weight) {
    monitorIn();
    while (curWeight + weight > maxWeight) {
      ++nbWaiting;
      wait(outbound);
      signal(outbound);
    }

    curWeight += weight;
    monitorOut();
  }

  void leave(float weight) {
    monitorIn();
    curWeight -= weight;
    if (nbWaiting > 0) {
      --nbWaiting;
      signal(outbound);
    }
    monitorOut();
  }
};
