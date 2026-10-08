#pragma once

#include <stdint.h>
#include <cstdlib>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <condition_variable>


const auto MAX_PROCESSOR_COUNT = std::thread::hardware_concurrency();

class ThreadPool
{
public:
    ThreadPool() = default;
    ~ThreadPool() = default;

    void SetupPool(size_t nThreads);
    void ClearPool();
    void Wait();

    template<typename F, typename... Args>
    void Enqueue(F&&f, Args&&... args);

private:
    size_t m_nThreads;

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    void Worker();

    std::mutex queueMutex;
    std::condition_variable condition;
    std::condition_variable finishedCondition;
    std::atomic<bool> stop{false};
    std::atomic<int> activeTasks{0};
};

//extern ThreadPool g_threadPool;

template <typename F, typename... Args>
inline void ThreadPool::Enqueue(F &&f, Args &&...args)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace([f = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable {
            if constexpr (std::is_member_function_pointer<F>::value)
                std::invoke(f, std::forward<Args>(args)...);
            else
                f(std::forward<Args>(args)...);
        });
        activeTasks++;
    }
    condition.notify_one();
}
