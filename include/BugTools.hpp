#ifndef BUGTOOLS
#define BUGTOOLS

#if defined(DEBUG)

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <sstream>
#include <atomic>

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void logMessage(const std::string& msg) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            logQueue.push(msg);
        }
        condition.notify_one();
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stopFlag = true;
        }
        condition.notify_one();
        if (loggingThread.joinable()) {
            loggingThread.join();
        }
    }

private:
    Logger() : stopFlag(false), loggingThread(&Logger::processLogs, this) {}

    ~Logger() {
        stop();
    }

    void processLogs() {
        while (true) {
            std::string message;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [this] { return stopFlag || !logQueue.empty(); });

                if (stopFlag && logQueue.empty()) return;

                message = std::move(logQueue.front());
                logQueue.pop();
            }
            std::cout << message << "\n";
        }
    }

    std::queue<std::string> logQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stopFlag;
    std::thread loggingThread;
};
//Logger::getInstance().logMessage(oss.str());
#define ASSERT(x, m) { if (!(x)) { std::cout << "[DEBUG_ASSERT]: " << m << "\n"; __debugbreak(); } }
#define LOG(m) { \
    std::ostringstream oss; \
    oss << "[DEBUG_LOG (" << __LINE__ << " | " << __FILE__ << ")]: " << m; \
    std::cout << oss.str() << "\n"; \
}

#else

#define ASSERT(x, m)
#define LOG(m)

#endif

#endif //BUGTOOLS