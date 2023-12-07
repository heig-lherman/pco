#include <pcosynchro/pcohoaremonitor.h>

class BridgeManagerFloat : protected PcoHoareMonitor {
private:
  PcoHoareMonitor::Condition outbound;
  float maxWeight, curWeight{0};

public:
  BridgeManagerFloat(float maxWeight) : maxWeight(maxWeight) {}
  ~BridgeManagerFloat() {}

  void access(float weight) {
    monitorIn();
    while (curWeight + weight > maxWeight) {
      wait(outbound);
    }

    curWeight += weight;
    signal(outbound);
    monitorOut();
  }

  void leave(float weight) {
    monitorIn();
    curWeight -= weight;
    signal(outbound);
    monitorOut();
  }
};
