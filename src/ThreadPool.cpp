#include "ThreadPool.hpp"
#include <iostream>

ThreadPool::ThreadPool(size_t numThreads) : stopFlag(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stopFlag = true;
    }
    condition.notify_all();

    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    workers.clear();
}

void ThreadPool::workerThread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stopFlag || !tasks.empty(); });

            if (stopFlag && tasks.empty()) return;

            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}
