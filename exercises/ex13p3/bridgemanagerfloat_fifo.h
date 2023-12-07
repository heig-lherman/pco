#include <pcosynchro/pcohoaremonitor.h>

class BridgeManagerFloat : protected PcoHoareMonitor {
private:
  PcoHoareMonitor::Condition outbound, accessing;
  float maxWeight, curWeight{0}, waitingWeight{0};
  bool isAccessing{false};

public:
  BridgeManagerFloat(float maxWeight) : maxWeight(maxWeight) {}
  ~BridgeManagerFloat() {}

  void access(float weight) {
    monitorIn();

    if (isAccessing) {
      wait(accessing);
    }

    isAccessing = true;
    if (curWeight + weight > maxWeight) {
      waitingWeight = weight;
      wait(outbound);
      waitingWeight = 0;
    }

    curWeight += weight;
    isAccessing = false;
    signal(accessing);
    monitorOut();
  }

  void leave(float weight) {
    monitorIn();
    curWeight -= weight;
    if (curWeight + waitingWeight <= maxWeight) {
      signal(outbound);
    }

    monitorOut();
  }
};
