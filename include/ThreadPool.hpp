#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    template <typename Func, typename... Args>
    void enqueue(Func&& func, Args&&... args);

    void stop();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stopFlag;

    void workerThread();
};

template <typename Func, typename... Args>
void ThreadPool::enqueue(Func&& func, Args&&... args) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (stopFlag) return;

        tasks.emplace([func = std::forward<Func>(func), ...args = std::forward<Args>(args)]() {
            func(args...);
        });
    }
    condition.notify_one();
}

#endif // THREAD_POOL_H
