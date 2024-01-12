
#include <gtest/gtest.h>

#include "pcotest.h"

#include "computationmanager.h"
#include "testcomputengine.h"

TEST(Pass, AlwaysPass) {
  ASSERT_EQ(1,1);
}

TEST(Global, ComputeEngineShouldWait_A) {

    ASSERT_DURATION_GE(1, {
        ComputationManager cm;
        cm.getWork(ComputationType::A);
                       })
}

TEST(Global, ComputeEngineShouldWait_B) {

    ASSERT_DURATION_GE(1, {
        ComputationManager cm;
        cm.getWork(ComputationType::B);
                       })
}

TEST(Global, ComputeEngineShouldWait_C) {

    ASSERT_DURATION_GE(1, {
        ComputationManager cm;
        cm.getWork(ComputationType::C);
                       })
}

TEST(Etape1, RequestComputationShouldBlockOnFullQueue) {

    ASSERT_DURATION_GE(1, {
                           // A computation manager buffer with max 2 elements in queues
                           ComputationManager cm(2);
                           cm.requestComputation(Computation(ComputationType::A));
                           cm.requestComputation(Computation(ComputationType::A));
                           // Should block (wait)
                           cm.requestComputation(Computation(ComputationType::A));
                       })

}

/* When the queues can contain 2 elements we should block with the third deposit */
TEST(Etape1, RequestComputationShouldBlockOnFullQueue2) {
    ASSERT_DURATION_GE(1, {
        ComputationManager cm(2);
        cm.requestComputation(Computation(ComputationType::B));
        cm.requestComputation(Computation(ComputationType::B));
        cm.requestComputation(Computation(ComputationType::B));
    })
}

/* When the queues can hold two items we should at least be able to deposit two in each */
TEST(Etape1, ShouldNotBlockOnNonFullQueues) {
    ASSERT_DURATION_LE(1, {
        ComputationManager cm(2);
        cm.requestComputation(Computation(ComputationType::A));
        cm.requestComputation(Computation(ComputationType::B));
        cm.requestComputation(Computation(ComputationType::B));
        cm.requestComputation(Computation(ComputationType::C));
        cm.requestComputation(Computation(ComputationType::C));
        cm.requestComputation(Computation(ComputationType::A));
    })
}

TEST(Etape1, WorkShouldPass) {
    ASSERT_DURATION_LE(1, {
                           // Work should go throug correctly
                           ComputationManager cm(2);
                           auto id = cm.requestComputation(Computation(ComputationType::A));
                           auto req = cm.getWork(ComputationType::A);
                           ASSERT_EQ(id, req.getId());
                       })
}

TEST(Etape1, WorkerShouldWait) {
    ASSERT_DURATION_GE(1, {
                           ComputationManager cm(2);
                           cm.getWork(ComputationType::A);
                       })
}

TEST(Etape2, AResultShouldArrive) {
    ASSERT_DURATION_LE(1, {
                           // Work should go through correctly
                           ComputationManager cm(2);
                           auto id = cm.requestComputation(Computation(ComputationType::A));
                           auto req = cm.getWork(ComputationType::A);
                           double value = 3.1415;
                           cm.provideResult(Result(id, value));
                           auto res = cm.getNextResult();
                           ASSERT_EQ(id, res.getId());
                           ASSERT_EQ(value, res.getResult());
                       })
}

TEST(Etape2, OrderShouldBeCorrect) {
    ASSERT_DURATION_LE(1, {
                           // Work should go through correctly
                           ComputationManager cm(2);
                           auto id1 = cm.requestComputation(Computation(ComputationType::A));
                           auto id2 = cm.requestComputation(Computation(ComputationType::A));
                           auto req1 = cm.getWork(ComputationType::A);
                           auto req2 = cm.getWork(ComputationType::A);
                           ASSERT_EQ(id1, req1.getId()) << "The first request should have the first id";
                           ASSERT_EQ(id2, req2.getId()) << "The second request should have the second id";
                           ASSERT_LT(req1.getId(), req2.getId()) << "The if of the first request should be lower than the one of the second request";
                           double value1 = 3.1415;
                           double value2 = 2.0;
                           cm.provideResult(Result(id2, value2));
                           cm.provideResult(Result(id1, value1));
                           auto res1 = cm.getNextResult();
                           auto res2 = cm.getNextResult();
                           ASSERT_EQ(id1, res1.getId()) << "The first result should have the first id";
                           ASSERT_EQ(id2, res2.getId()) << "The second result should have the second id";
                           ASSERT_EQ(value1, res1.getResult()) << "The values of the results should not be mixed";
                           ASSERT_EQ(value2, res2.getResult()) << "The values of the results should not be mixed";
                       })
}

TEST(Etape3, WorkerShouldNotContinueWorkingOnAbortedTask) {
    ASSERT_DURATION_LE(1, {
                           ComputationManager cm(2);
                           auto id = cm.requestComputation(Computation(ComputationType::A));
                           auto req = cm.getWork(ComputationType::A);
                           ASSERT_TRUE(cm.continueWork(req.getId())) << "Worker should continue working unless aborted";
                           cm.abortComputation(id);
                           ASSERT_FALSE(cm.continueWork(req.getId())) << "Worker should stop working on task when aborted";
                       })
}

TEST(Etape3, WorkerShouldNotGetAbortedTask) {
    ASSERT_DURATION_GE(1, {
                           ComputationManager cm(2);
                           auto id = cm.requestComputation(Computation(ComputationType::A));
                           cm.abortComputation(id);
                           // Should wait since there should be no work
                           auto req = cm.getWork(ComputationType::A);
                       })
}

TEST(Etape3, AbortedResultShouldNotComeBack) {
    ASSERT_DURATION_GE(1, {
                           ComputationManager cm(2);
                           auto id = cm.requestComputation(Computation(ComputationType::A));
                           // Should wait since there should be no work
                           auto req = cm.getWork(ComputationType::A);
                           cm.provideResult(Result(req.getId(), 0.0));
                           cm.abortComputation(id);
                           // Should block
                           cm.getNextResult();
                       })
}

TEST(Etape3, AbortShouldReleaseClientWaitingOnFullQueue) {
    ASSERT_DURATION_LE(1, {
        ComputationManager cm(2);
        auto id = cm.requestComputation(Computation(ComputationType::A));
        cm.requestComputation(Computation(ComputationType::A));
        // The thread will wait because there is no space inside the queue
        auto t = std::thread([&](){cm.requestComputation(Computation(ComputationType::A));});
        // Small delay to ensure the thread above did the request
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // Abort the first computation, should make space for the third request
        cm.abortComputation(id);
        // Thread should be able to deposit and thus finish
        t.join();
    })
}

TEST(Etape4, BufferShouldThrowException) {
    ASSERT_DURATION_LE(1, {
        auto cm = std::make_shared<ComputationManager>(2);

        cm->stop();

        try {
            cm->getNextResult();
            ASSERT_TRUE(false) << "Buffer should have thrown exception";
        } catch (ComputationManager::StopException& e) {
        }
    })
}

TEST(Etape4, BufferShouldThrowException2) {
    ASSERT_DURATION_LE(1, {
        auto cm = std::make_shared<ComputationManager>(2);

        cm->stop();

        try {
            cm->getWork(ComputationType::A);
            ASSERT_TRUE(false) << "Buffer should have thrown exception";
        } catch (ComputationManager::StopException& e) {
        }
    })
}

TEST(Etape4, ClientShouldBeReleased) {
    ASSERT_DURATION_LE(1, {
        auto cm = std::make_shared<ComputationManager>(2);

        // This thread will block
        auto thread = std::thread([=](){
            try {
                cm->getNextResult();
                ASSERT_TRUE(false) << "Buffer should have thrown exception";
            } catch (ComputationManager::StopException& e) {
            }
        });

        // Should release the thread
        cm->stop();

        // Thread should be able to finish
        thread.join();
    })
}

TEST(Etape4, ClientShouldBeReleased2) {
    ASSERT_DURATION_LE(1, {
        auto cm = std::make_shared<ComputationManager>(2);

        // This thread will block
        auto thread = std::thread([=](){
            try {
                cm->requestComputation(Computation(ComputationType::A));
                cm->requestComputation(Computation(ComputationType::A));
                // Should block here
                cm->requestComputation(Computation(ComputationType::A));
                cm->requestComputation(Computation(ComputationType::A));
                ASSERT_TRUE(false) << "Buffer should have thrown exception";
            } catch (ComputationManager::StopException& e) {
            }
        });

        // Small delay to ensure the thread above did the requests
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Should release the thread
        cm->stop();

        // Thread should be able to finish
        thread.join();
    })
}

TEST(Etape4, ComputeEngineShouldBeReleased) {
    ASSERT_DURATION_LE(1, {
        auto cm = std::make_shared<ComputationManager>(2);

        // This thread will block
        auto thread = std::thread([=](){
            try {
                cm->getWork(ComputationType::A);
                ASSERT_TRUE(false) << "getWork() should have blocked and/or buffer should have thrown exception";
            } catch (ComputationManager::StopException& e) {
            }
        });

        // Small delay to ensure the thread above did the requests
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Should release the thread
        cm->stop();

        // Thread should be able to finish
        thread.join();
    })
}

/* The results should arrive in the correct order and hold the correct value */
TEST(Etape4, ResultsShouldArriveInOrderAndBufferMustStop) {
    ASSERT_DURATION_LE(2 , {
    auto cm = std::make_shared<ComputationManager>();

    // Test compute engines return the id casted to double as the result
    std::vector<TestComputeEngine> tce;
    tce.push_back(TestComputeEngine(cm, ComputationType::A, 10, 10));
    tce.push_back(TestComputeEngine(cm, ComputationType::A, 12, 10));
    tce.push_back(TestComputeEngine(cm, ComputationType::B, 4, 10));
    tce.push_back(TestComputeEngine(cm, ComputationType::C, 2, 10));

    for (auto& t : tce) {
        t.startThread();
    }

    for (int i = 0; i < 10; ++i) {
        cm->requestComputation(Computation(ComputationType::A));
        cm->requestComputation(Computation(ComputationType::B));
        cm->requestComputation(Computation(ComputationType::C));
    }
    auto oldResult = cm->getNextResult();
    for (int i = 0; i < 30-1; ++i) {
        auto newResult = cm->getNextResult();
        ASSERT_LT(oldResult.getId(), newResult.getId()) << "The results should arrive in order";
        // The test compue engine result is the Id
        ASSERT_LT(oldResult.getResult(), newResult.getResult()) << "The results should not be mixed";
        oldResult = newResult;
    }

    cm->stop();
    for (auto& t : tce) {
        t.join();
    }
    })
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
