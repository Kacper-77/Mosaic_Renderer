#include "job_system.h"

MosaicJobSystem::MosaicJobSystem() {
    unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4;
    if (numThreads > 1) numThreads -= 1;

    m_workers.resize(numThreads);
    for (unsigned int i = 0; i < numThreads; ++i) {
        m_workers[i].id = i;
        m_workers[i].thread = std::thread(&MosaicJobSystem::WorkerLoop, this, i);
    }
}

MosaicJobSystem::~MosaicJobSystem() {
    m_shutdown = true;
    m_kickSignal.notify_all();
    for (auto& worker : m_workers) {
        if (worker.thread.joinable()) {
            worker.thread.join();
        }
    }
}

void MosaicJobSystem::Dispatch(size_t totalJobs, const std::function<void(size_t, int)>& jobTarget) {
    if (totalJobs == 0) return;

    m_totalJobs.store(totalJobs, std::memory_order_relaxed);
    m_nextJobIndex.store(0, std::memory_order_release);
    m_currentJobTarget = jobTarget;
    m_activeWorkers.store(static_cast<int>(m_workers.size()), std::memory_order_release);

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_workersRunning = true;
    }
    
    m_kickSignal.notify_all();

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_doneSignal.wait(lock, [this]() { return !m_workersRunning.load(); });
    }
}

void MosaicJobSystem::WorkerLoop(int workerId) {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_kickSignal.wait(lock, [this]() { 
                return m_workersRunning.load() || m_shutdown.load(); 
            });
        }

        if (m_shutdown) break;

        const size_t total = m_totalJobs.load(std::memory_order_relaxed);
        
        while (true) {
            size_t jobIdx = m_nextJobIndex.fetch_add(1, std::memory_order_relaxed);
            if (jobIdx >= total) break;

            m_currentJobTarget(jobIdx, workerId);
        }

        int remaining = m_activeWorkers.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (remaining == 0) {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_workersRunning = false;
            }
            m_doneSignal.notify_one();
        }
    }
}
