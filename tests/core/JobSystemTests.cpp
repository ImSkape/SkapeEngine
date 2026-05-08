//-----------------------------------
// JobSystemTests.cpp
// Caleb Davis
// Unit tests for JobSystem
//-------------------------------------

#include <catch2/catch_all.hpp>
#include "engine/core/JobSystem.h"
#include <atomic>
#include <chrono>
#include <vector>

TEST_CASE("JobSystem: Init and Shutdown", "[jobs]") {

    SECTION("initializes successfully") {
        JobSystem jobs;
        SEResult result = jobs.Init();
        REQUIRE(result.ok());
        REQUIRE(jobs.GetWorkerCount() > 0);
        jobs.Shutdown();
    }

    SECTION("worker count matches hardware minus one") {
        JobSystem jobs;
        jobs.Init();
        uint32_t expected = std::max(1u,
            std::thread::hardware_concurrency() - 1);
        REQUIRE(jobs.GetWorkerCount() == expected);
        jobs.Shutdown();
    }

    SECTION("can shutdown and reinitialize") {
        JobSystem jobs;
        jobs.Init();
        jobs.Shutdown();
        SEResult result = jobs.Init();
        REQUIRE(result.ok());
        jobs.Shutdown();
    }
}

TEST_CASE("JobSystem: Single job", "[jobs]") {
    JobSystem jobs;
    jobs.Init();

    SECTION("job executes") {
        std::atomic<bool> ran { false };
        JobHandle handle = jobs.Submit([&ran]() {
            ran = true;
        });
        jobs.Wait(handle);
        REQUIRE(ran.load());
    }

    SECTION("job executes correct work") {
        std::atomic<int> result { 0 };
        JobHandle handle = jobs.Submit([&result]() {
            result = 42;
        });
        jobs.Wait(handle);
        REQUIRE(result.load() == 42);
    }

    SECTION("handle is done after wait") {
        JobHandle handle = jobs.Submit([]() {});
        jobs.Wait(handle);
        REQUIRE(handle.IsDone());
    }

    SECTION("multiple sequential jobs execute in order") {
        std::vector<int> order;
        std::mutex orderMutex;

        for (int i = 0; i < 5; i++) {
            JobHandle handle = jobs.Submit([i, &order, &orderMutex]() {
                std::lock_guard<std::mutex> lock(orderMutex);
                order.push_back(i);
            });
            jobs.Wait(handle);
        }

        REQUIRE(order.size() == 5);
        for (int i = 0; i < 5; i++)
            REQUIRE(order[i] == i);
    }

    jobs.Shutdown();
}

TEST_CASE("JobSystem: Batch jobs", "[jobs]") {
    JobSystem jobs;
    jobs.Init();

    SECTION("all batch jobs execute") {
        std::atomic<int> counter { 0 };

        std::vector<Job> batch;
        for (int i = 0; i < 10; i++) {
            batch.push_back([&counter]() {
                counter.fetch_add(1);
            });
        }

        JobHandle handle = jobs.Submit(std::move(batch));
        jobs.Wait(handle);
        REQUIRE(counter.load() == 10);
    }

    SECTION("empty batch returns done handle") {
        JobHandle handle = jobs.Submit(std::vector<Job>{});
        REQUIRE(handle.IsDone());
    }

    SECTION("batch handle not done until all jobs complete") {
        std::atomic<int> counter { 0 };
        const int jobCount = 100;

        std::vector<Job> batch;
        for (int i = 0; i < jobCount; i++) {
            batch.push_back([&counter]() {
                counter.fetch_add(1);
            });
        }

        JobHandle handle = jobs.Submit(std::move(batch));
        jobs.Wait(handle);

        REQUIRE(handle.IsDone());
        REQUIRE(counter.load() == jobCount);
    }

    SECTION("large batch all execute") {
        std::atomic<int> counter { 0 };
        const int jobCount = 1000;

        std::vector<Job> batch;
        for (int i = 0; i < jobCount; i++) {
            batch.push_back([&counter]() {
                counter.fetch_add(1);
            });
        }

        JobHandle handle = jobs.Submit(std::move(batch));
        jobs.Wait(handle);
        REQUIRE(counter.load() == jobCount);
    }

    jobs.Shutdown();
}

TEST_CASE("JobSystem: JobHandle", "[jobs]") {
    JobSystem jobs;
    jobs.Init();

    SECTION("default handle is done") {
        JobHandle handle;
        REQUIRE(handle.IsDone());
    }

    SECTION("handle with counter zero is done") {
        JobHandle handle;
        handle.counter = std::make_shared<std::atomic<int>>(0);
        REQUIRE(handle.IsDone());
    }

    SECTION("handle with counter above zero is not done") {
        JobHandle handle;
        handle.counter = std::make_shared<std::atomic<int>>(1);
        REQUIRE_FALSE(handle.IsDone());
    }

    SECTION("copied handle shares counter") {
        std::atomic<bool> ran { false };
        JobHandle original = jobs.Submit([&ran]() {
            ran = true;
        });
        JobHandle copy = original;
        jobs.Wait(original);
        REQUIRE(copy.IsDone());
    }

    jobs.Shutdown();
}

TEST_CASE("JobSystem: Concurrent jobs", "[jobs]") {
    JobSystem jobs;
    jobs.Init();

    SECTION("concurrent jobs do not corrupt shared state") {
        std::atomic<int> counter { 0 };
        const int jobCount = 500;

        std::vector<Job> batch;
        for (int i = 0; i < jobCount; i++) {
            batch.push_back([&counter]() {
                counter.fetch_add(1);
            });
        }

        JobHandle handle = jobs.Submit(std::move(batch));
        jobs.Wait(handle);
        REQUIRE(counter.load() == jobCount);
    }

    SECTION("multiple batches submitted concurrently all complete") {
        std::atomic<int> counter { 0 };
        const int batchCount = 4;
        const int jobsPerBatch = 50;

        std::vector<JobHandle> handles;

        for (int b = 0; b < batchCount; b++) {
            std::vector<Job> batch;
            for (int j = 0; j < jobsPerBatch; j++) {
                batch.push_back([&counter]() {
                    counter.fetch_add(1);
                });
            }
            handles.push_back(jobs.Submit(std::move(batch)));
        }

        for (auto& handle : handles)
            jobs.Wait(handle);

        REQUIRE(counter.load() == batchCount * jobsPerBatch);
    }

    SECTION("jobs can submit other jobs") {
        std::atomic<int> counter { 0 };

        JobHandle outer = jobs.Submit([&jobs, &counter]() {
            // job submits another job
            JobHandle inner = jobs.Submit([&counter]() {
                counter.fetch_add(1);
            });
            jobs.Wait(inner);
            counter.fetch_add(1);
        });

        jobs.Wait(outer);
        REQUIRE(counter.load() == 2);
    }

    jobs.Shutdown();
}

TEST_CASE("JobSystem: Wait behavior", "[jobs]") {
    JobSystem jobs;
    jobs.Init();

    SECTION("wait on already done handle returns immediately") {
        JobHandle handle = jobs.Submit([]() {});
        jobs.Wait(handle);
        REQUIRE(handle.IsDone());

        // waiting again on done handle should be safe
        jobs.Wait(handle);
        REQUIRE(handle.IsDone());
    }

    SECTION("wait on default handle is safe") {
        JobHandle handle;
        REQUIRE_NOTHROW(jobs.Wait(handle));
    }

    SECTION("results are visible after wait") {
        std::atomic<int> value { 0 };

        JobHandle handle = jobs.Submit([&value]() {
            // simulate some work
            for (int i = 0; i < 1000; i++)
                value.fetch_add(1);
        });

        jobs.Wait(handle);
        REQUIRE(value.load() == 1000);
    }

    jobs.Shutdown();
}