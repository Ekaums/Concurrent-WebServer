#include "threadpool.h"
#include "atomic_writer.h"

Threadpool::Threadpool(size_t num_threads){

    for(int i = 0; i < num_threads; i++){
        threads.emplace_back(std::thread(&Threadpool::threadloop, this));
        Atomic_cout() << "thread " << threads[i].get_id() << " created" << std::endl;
    }
}

void Threadpool::start(){
    
    while(true){
    Atomic_cout() << "waiting to continue..." << std::endl;
    std::cin.get();
    std::unique_lock<std::mutex> lock(queue_mutex);
    ready = true;
    mutex_condition.notify_one();
    lock.unlock();
    }

    std::cout << "done" << std::endl;
}

// Waiting loop for threads
void Threadpool::threadloop(void){
    while(true){
    std::unique_lock<std::mutex> lock(queue_mutex);

    Atomic_cout() << "Thread: " << std::this_thread::get_id() << " is waiting" << std::endl;
    mutex_condition.wait(lock, [this]{return ready;});

    Atomic_cout() << "Thread: " << std::this_thread::get_id() << " is doing work" << std::endl;
    ready = false;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    lock.unlock();
    }
}

