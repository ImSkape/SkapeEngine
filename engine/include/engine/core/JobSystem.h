//-----------------------------------
// JobSystem.h
// Caleb Davis
// Handles parralelization for engine
//-------------------------------------

#pragma once
#include "engine/core/System.h"
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <deque>
#include <cstdint>
#include <string>


// a job is just a callable with no arguments or return value
// return values are communicated via captured references or shared state
//TODO: less overhead
using Job = std::function<void()>;

// handle returned when submitting jobs
// use with Wait() to block until jobs complete
struct JobHandle {
    std::shared_ptr<std::atomic<int>> counter;

    bool IsDone() const {
        return counter && counter->load() == 0;
    }
};

class JobSystem : public System {
public:
    SEResult Init() override;
    void     Shutdown() override;

    // submit a single job — returns handle to wait on
    JobHandle Submit(Job job);

    // submit a batch of jobs — all share one handle
    JobHandle Submit(std::vector<Job> jobs);

    // block calling thread until handle is done
    void Wait(const JobHandle& handle);

    // how many worker threads are running
    uint32_t GetWorkerCount() const { return (uint32_t)m_threads.size(); }

    const std::string& GetName() const override {
        static std::string name = "JobSystem";
        return name;
    }

private:
    // one queue per worker thread
    struct WorkerQueue {
        std::deque<Job>         jobs;
        std::mutex              mutex;
        std::condition_variable cv;
    };

    std::vector<std::thread>                  m_threads;
    std::vector<std::unique_ptr<WorkerQueue>> m_queues;
    std::atomic<bool>                         m_running{ false };
    std::atomic<uint32_t>                     m_nextQueue{ 0 };

    void     WorkerThread(uint32_t index);
    bool     TrySteal(uint32_t thiefIndex, Job& outJob);
    uint32_t GetNextQueueIndex();
};