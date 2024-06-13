#include "threadpool.h"
#include "atomic_writer.h"
#include "request.h"
#include "server_helper.h"


// TODO: Use seperate locks for pushing/popping queue

Threadpool::Threadpool(size_t num_threads, size_t buf_size) : queue_size(buf_size)
{
    for(size_t i = 0; i < num_threads; i++){
        threads.emplace_back(std::thread(&Threadpool::threadloop, this));
        Atomic_cout() << "thread " << threads[i].get_id() << " created" << std::endl;
    }
}

void Threadpool::threadloop(void){
    while(true){
    std::unique_lock<std::mutex> lock(queue_mutex);

    Atomic_cout() << "Thread: " << std::this_thread::get_id() << " ready" << std::endl;
    job.wait(lock, [this]{return !jobs.empty();}); // Wait until job is in queue (then gets signalled)

    int job = jobs.front();
    jobs.pop();
    Atomic_cout() << "Thread: " << std::this_thread::get_id() << " handling " << job << std::endl;
    empty.notify_one();
    lock.unlock();
    processJob(job);
    }
}

void Threadpool::queueJob(int fd){
    std::unique_lock<std::mutex> lock(queue_mutex);
    Atomic_cout() << "jobs in queue: " << jobs.size() << std::endl;
    empty.wait(lock, [this]{return (jobs.size() != queue_size);}); // Ensure that queue is not full. If it is, wait to be signalled (by workers) that there is an empty spot
    jobs.push(fd);
    Atomic_cout() << "Added " << fd << " to queue" << std::endl;
    Atomic_cout() << "jobs queue size: " << jobs.size() << std::endl;
    job.notify_one();
    lock.unlock();
}

void Threadpool::processJob(int fd){
    handle_request(fd);
    Atomic_cout() << "Thread: " << std::this_thread::get_id() << " completed " << fd << std::endl;
    close_or_die(fd);
}

