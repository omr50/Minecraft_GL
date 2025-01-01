#pragma once
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <vector>
#include <functional>
#include <condition_variable>

class ThreadPool
{
public:
    std::mutex task_mutex;
    std::condition_variable conditional;
    std::atomic<bool> stop;
    std::queue<std::function<void()>> task_queue;
    std::vector<std::thread> workers;

    ThreadPool(int num_threads);
    void enqueue_task(std::function<void()> task);
    // DO THIS LATER
    void enqueue_mesh_function(std::function<void()> task);
    void worker_loop();
};