//-----------------------------------
// JobSystem.cpp
// Caleb Davis
// Handles parralelization for engine
//-------------------------------------
#include "engine/core/JobSystem.h"
#include <cstdio>
#include <cassert>


SEResult JobSystem::Init() {
    assert(!m_running && "JobSystem already initialized");

    uint32_t threadCount = std::max(1u, std::thread::hardware_concurrency() - 1);

    m_running = true;

    // create one queue per worker thread
    for (uint32_t i = 0; i < threadCount; i++)
        m_queues.push_back(std::make_unique<WorkerQueue>());

    // create and start worker threads
    for (uint32_t i = 0; i < threadCount; i++)
        m_threads.emplace_back(&JobSystem::WorkerThread, this, i);

    m_initialized = true;
    printf("[JobSystem] %u worker threads started\n", threadCount);
    return SEResult::ok();
}


void JobSystem::Shutdown() {
    // signal all threads to stop
    m_running = false;

    // wake every sleeping thread so they can see m_running is false
    for (auto& queue : m_queues) {
        std::lock_guard<std::mutex> lock(queue->mutex);
        queue->cv.notify_all();
    }

    // wait for every thread to finish its current job and exit
    for (auto& thread : m_threads) {
        if (thread.joinable())
            thread.join();
    }

    m_threads.clear();
    m_queues.clear();
    m_initialized = false;

    printf("[JobSystem] Shutdown\n");
}


JobHandle JobSystem::Submit(Job job) {
    // create handle with counter set to 1
    JobHandle handle;
    handle.counter = std::make_shared<std::atomic<int>>(1);

    // capture counter so job decrements it when done
    auto counter = handle.counter;
    Job wrappedJob = [job = std::move(job), counter]() {
        job();
        counter->fetch_sub(1);  // decrement - if hits 0 job is done
        counter->notify_all();  // wake anyone waiting on this handle
        };

    // pick a queue round-robin and push the job
    uint32_t index = GetNextQueueIndex();
    {
        std::lock_guard<std::mutex> lock(m_queues[index]->mutex);
        m_queues[index]->jobs.push_back(std::move(wrappedJob));
    }
    m_queues[index]->cv.notify_one();  // wake a sleeping thread

    return handle;
}

JobHandle JobSystem::Submit(std::vector<Job> jobs) {
    if (jobs.empty()) {
        // return an already-done handle
        JobHandle handle;
        handle.counter = std::make_shared<std::atomic<int>>(0);
        return handle;
    }

    // counter starts at number of jobs — decrements as each completes
    JobHandle handle;
    handle.counter = std::make_shared<std::atomic<int>>((int)jobs.size());

    auto counter = handle.counter;

    for (auto& job : jobs) {
        Job wrappedJob = [j = std::move(job), counter]() {
            j();
            counter->fetch_sub(1);
            counter->notify_all();
            };

        uint32_t index = GetNextQueueIndex();
        {
            std::lock_guard<std::mutex> lock(m_queues[index]->mutex);
            m_queues[index]->jobs.push_back(std::move(wrappedJob));
        }
        m_queues[index]->cv.notify_one();
    }

    return handle;
}

void JobSystem::Wait(const JobHandle& handle) {
    if (!handle.counter) return;

    // block until counter hits zero
    // atomic::wait() sleeps the thread until the value changes
    while (handle.counter->load() > 0)
        handle.counter->wait(handle.counter->load());
}

void JobSystem::WorkerThread(uint32_t index) {
    WorkerQueue& myQueue = *m_queues[index];

    while (m_running) {
        Job job;
        bool gotJob = false;

        // try own queue first
        {
            std::lock_guard<std::mutex> lock(myQueue.mutex);
            if (!myQueue.jobs.empty()) {
                job = std::move(myQueue.jobs.front());
                myQueue.jobs.pop_front();
                gotJob = true;
            }
        }

        // if own queue empty try stealing from another thread
        if (!gotJob)
            gotJob = TrySteal(index, job);

        if (gotJob) {
            job();  // execute the job
        }
        else {
            // nothing to do — sleep until woken
            std::unique_lock<std::mutex> lock(myQueue.mutex);
            myQueue.cv.wait(lock, [&] {
                return !myQueue.jobs.empty() || !m_running;
                });
        }
    }

    // drain remaining jobs before exiting
    // important — don't leave jobs unfinished when shutting down
    while (true) {
        Job job;
        bool gotJob = false;

        {
            std::lock_guard<std::mutex> lock(myQueue.mutex);
            if (!myQueue.jobs.empty()) {
                job = std::move(myQueue.jobs.front());
                myQueue.jobs.pop_front();
                gotJob = true;
            }
        }

        if (!gotJob) break;
        job();
    }
}

bool JobSystem::TrySteal(uint32_t thiefIndex, Job& outJob) {
    // try each queue except our own
    for (uint32_t i = 0; i < m_queues.size(); i++) {
        if (i == thiefIndex) continue;

        std::lock_guard<std::mutex> lock(m_queues[i]->mutex);
        if (!m_queues[i]->jobs.empty()) {
            // steal from the back — victim pops from front
            // minimises contention between thief and victim
            outJob = std::move(m_queues[i]->jobs.back());
            m_queues[i]->jobs.pop_back();
            return true;
        }
    }
    return false;
}

uint32_t JobSystem::GetNextQueueIndex() {
    // atomic increment wraps around queue count
    return m_nextQueue.fetch_add(1) % (uint32_t)m_queues.size();
}