#pragma once

#include <sys/types.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>


class Threadpool{

    private:
    std::vector<std::thread> threads;
    std::mutex queue_mutex;
    std::condition_variable mutex_condition;
    bool ready = false;


    public:
    // Create threads
    Threadpool(size_t num_threads);

    // Put a thread to work
    void start(void);

    // Thread waiting room
    void threadloop(void);

};