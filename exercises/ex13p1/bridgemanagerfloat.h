
#include <pcosynchro/pcosemaphore.h>


class BridgeManagerFloat
{
  private: 
    PcoSemaphore wait{0}, block{1}, fifo{1};
    float maxWeight, curWeight{0};
    unsigned nbWaiting{0};

  public:
    BridgeManagerFloat(float maxWeight): maxWeight(maxWeight) {}
    ~BridgeManagerFloat() {} 

    void access(float weight) {
      fifo.acquire();
      block.acquire();
      while (curWeight + weight > maxWeight) {
        nbWaiting++;
        block.release();
        wait.acquire();
        block.acquire();
      }
      curWeight += weight;
      block.release();
      fifo.release();
    }

    void leave(float weight) {
      block.acquire();
      curWeight -= weight;
      for (int i = 0; i < nbWaiting; ++i) {
        wait.release();
      }
      nbWaiting = 0;
      block.release();
    }
};
