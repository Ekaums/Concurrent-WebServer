#pragma once

#include <sys/types.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <queue>


class Threadpool{

    private:
    std::vector<std::thread> threads; // Pool of worker threads
    std::queue<int> jobs; // Holds incoming requests
    size_t queue_size;
    std::mutex queue_mutex;
    std::condition_variable empty, job; // Signals to wake the master if a request has been consumed (and thus there is space in the queue), and signal to consumers when the queue has a job. Respectfully

    // Processing loop for threads
    void threadloop(void);

    // Handle request (where the magic happens)
    void processJob(int fd);

    public:
    // Create threads
    Threadpool(size_t num_threads, size_t buf_size);

    void queueJob(int fd);
};