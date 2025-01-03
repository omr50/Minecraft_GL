#include "../../include/ThreadPool.hpp"
#include <iostream>

ThreadPool::ThreadPool(int num_threads)
{
    // create a bunch of workers that each run the worker loop
    for (int i = 0; i < num_threads; i++)
    {
        workers.emplace_back([this]()
                             { this->worker_loop(); });
    }
}

void ThreadPool::enqueue_task(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex>
            lock(task_mutex);
        task_queue.push(std::move(task));
    }

    conditional.notify_one();
}

void ThreadPool::worker_loop()
{
    // infinite loop to keep picking up tasks
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(task_mutex);
            conditional.wait(lock, [this]
                             { return !task_queue.empty() || stop; });

            // if (stop && task_queue.empty())
            // return;

            task = std::move(task_queue.front());
            task_queue.pop();
        }
        // printf("doing unique task!\n");
        task();
    }
}