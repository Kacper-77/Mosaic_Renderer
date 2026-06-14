#pragma once

#include <thread>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <functional>

class MosaicJobSystem {
private:
    struct WorkerData {
        alignas(64) std::thread thread;
        int id = 0;
    };

    std::vector<WorkerData> m_workers;
    
    std::atomic<size_t> m_nextJobIndex{0};
    std::atomic<size_t> m_totalJobs{0};
    std::atomic<int>    m_activeWorkers{0};
    std::atomic<bool>   m_workersRunning{false};
    std::atomic<bool>   m_shutdown{false};

    std::mutex              m_mutex;
    std::condition_variable m_kickSignal;
    std::condition_variable m_doneSignal;

    std::function<void(size_t, int)> m_currentJobTarget;

    void WorkerLoop(int workerId);

public:
    MosaicJobSystem();
    ~MosaicJobSystem();

    // Copy disabled - Job System 1 of 1
    MosaicJobSystem(const MosaicJobSystem&) = delete;
    MosaicJobSystem& operator=(const MosaicJobSystem&) = delete;

    void Dispatch(size_t totalJobs, const std::function<void(size_t jobIdx, int workerId)>& jobTarget);

    size_t GetWorkerCount() const { return m_workers.size(); }
};
