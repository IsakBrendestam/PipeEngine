#include "stdafx.h"
#include "ThreadPool.h"

#include <algorithm>

//ThreadPool g_threadPool;

void ThreadPool::SetupPool(size_t nThreads)
{
    m_nThreads = nThreads;
    stop = false;
    for (size_t i = 0; i < nThreads; i++)
        workers.emplace_back(&ThreadPool::Worker, this);
}

void ThreadPool::ClearPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
        condition.notify_all();
    }

    for (auto& worker : workers)
        if (worker.joinable())
            worker.join();

    workers.clear();

    std::queue<std::function<void()>> empty;
    std::swap(tasks, empty);

    activeTasks = 0;
    finishedCondition.notify_all();
}

void ThreadPool::Wait()
{
    std::unique_lock<std::mutex> lock(queueMutex);

    finishedCondition.wait(lock, [this]() {
        return stop || (tasks.empty() && activeTasks == 0);
    });
}

void ThreadPool::Worker()
{
    while (true)
    {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() {
                return stop || !tasks.empty();
            });

            if (stop)
                return;

            task = std::move(tasks.front());
            tasks.pop();
        }

        task();

        activeTasks--;
        finishedCondition.notify_one();
    }
}
