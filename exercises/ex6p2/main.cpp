
#include <algorithm>
#include <iostream>

#include <gtest/gtest.h>

#include <pcosynchro/pcologger.h>
#include <pcosynchro/pcosemaphore.h>
#include <pcosynchro/pcothread.h>

class Checker {
public:
  std::vector<int> executions;

  void compute(int id) {
    static std::mutex mutex;
    mutex.lock();
    executions.push_back(id);
    mutex.unlock();
    logger() << "Task " << id << std::endl;
  }

  bool executedBefore(int id0, int id1) {
    auto it0 = find(executions.begin(), executions.end(), id0);
    auto it1 = find(executions.begin(), executions.end(), id1);
    return it0 < it1;
  }

  void check() {
    EXPECT_TRUE(executedBefore(0, 1));
    EXPECT_TRUE(executedBefore(0, 2));
    EXPECT_TRUE(executedBefore(1, 3));
    EXPECT_TRUE(executedBefore(1, 4));
    EXPECT_TRUE(executedBefore(1, 5));
    EXPECT_TRUE(executedBefore(3, 6));
    EXPECT_TRUE(executedBefore(4, 6));
    EXPECT_TRUE(executedBefore(5, 6));
    EXPECT_TRUE(executedBefore(2, 7));
    EXPECT_TRUE(executedBefore(6, 7));
  }
};

static Checker checker;
static PcoSemaphore sem0{0};
static PcoSemaphore sem1{0};
static PcoSemaphore sem345{0};
static PcoSemaphore sem26{0};

void t0() {
  checker.compute(0);
  sem0.release();
  sem0.release();
}

void t1() {
  sem0.acquire();
  checker.compute(1);
  sem1.release();
  sem1.release();
  sem1.release();
}

void t2() {
  sem0.acquire();
  checker.compute(2);
  sem26.release();
}

void t3() {
  sem1.acquire();
  checker.compute(3);
  sem345.release();
}

void t4() {
  sem1.acquire();
  checker.compute(4);
  sem345.release();
}

void t5() {
  sem1.acquire();
  checker.compute(5);
  sem345.release();
}

void t6() {
  sem345.acquire();
  sem345.acquire();
  sem345.acquire();
  checker.compute(6);
  sem26.release();
}

void t7() {
  sem26.acquire();
  sem26.acquire();
  checker.compute(7);
}

TEST(Sequentialite, Standard) {
  PcoManager::getInstance()->setMaxSleepDuration(
      1000, PcoManager::EventType::ThreadCreation);

  // Il s'agit ici d'un exemple qui n'est pas forcément la solution
  std::vector<std::unique_ptr<PcoThread>> threads(8);
  threads[0] = std::make_unique<PcoThread>(t0);
  threads[1] = std::make_unique<PcoThread>(t1);
  threads[2] = std::make_unique<PcoThread>(t2);
  threads[3] = std::make_unique<PcoThread>(t3);
  threads[4] = std::make_unique<PcoThread>(t4);
  threads[5] = std::make_unique<PcoThread>(t5);
  threads[6] = std::make_unique<PcoThread>(t6);
  threads[7] = std::make_unique<PcoThread>(t7);

  for (int i = 0; i < 8; i++) {
    threads[i]->join();
  }

  checker.check();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  logger().initialize(argc, argv);
  return RUN_ALL_TESTS();
}
