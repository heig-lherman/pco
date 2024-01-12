#ifndef PCOTEST_H
#define PCOTEST_H

#include <future>

// Taken here: https://github.com/google/googletest/issues/348
#define ASSERT_DURATION_LE(secs, stmt) { \
    std::promise<bool> completed; \
    auto stmt_future = completed.get_future(); \
    std::thread thread([&] { \
    stmt; \
    completed.set_value(true); \
    }); \
    if(stmt_future.wait_for(std::chrono::seconds(secs)) == std::future_status::timeout) { \
    thread.detach(); \
    GTEST_FATAL_FAILURE_("       timed out (> " #secs \
    " seconds). Check code for interlocking"); \
    } \
    else \
         thread.join(); \
    }

#define ASSERT_DURATION_GE(secs, stmt) { \
    std::promise<bool> completed; \
    auto stmt_future = completed.get_future(); \
    std::thread([&](std::promise<bool>& completed) { \
    stmt; \
    completed.set_value(true); \
    }, std::ref(completed)).detach(); \
    if(stmt_future.wait_for(std::chrono::seconds(secs)) != std::future_status::timeout) \
    GTEST_FATAL_FAILURE_("The code finished while it shouldn't"); \
    }


#endif // PCOTEST_H
