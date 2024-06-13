#pragma once

#include <vector>
#include <thread>

class Threadpool{

    private:
    int port;
    std::string hostname, filename;

    public:

    // Pool of client threads
    std::vector<std::thread> threads;
    
    // Creates and starts threads
    Threadpool(size_t num_threads, std::string server_addr, int server_port, std::string file);
    
    // Thread function
    void sendRequest(void);
};