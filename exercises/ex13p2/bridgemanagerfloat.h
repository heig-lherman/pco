
#include <pcosynchro/pcoconditionvariable.h>

class BridgeManagerFloat {
private:
  PcoMutex mutex;
  PcoConditionVariable outbound;
  float maxWeight, curWeight{0};

public:
  BridgeManagerFloat(float maxWeight) : maxWeight(maxWeight) {}
  ~BridgeManagerFloat() {}

  void access(float weight) {
    mutex.lock();
    while (curWeight + weight > maxWeight) {
      outbound.wait(&mutex);
    }

    curWeight += weight;
    mutex.unlock();
  }

  void leave(float weight) {
    mutex.lock();
    curWeight -= weight;
    outbound.notifyAll();
    mutex.unlock();
  }
};
