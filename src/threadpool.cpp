#include "threadpool.h"
#include "atomic_writer.h"

static bool ready = false;
static std::vector<std::thread> threads;
static std::mutex queue_mutex;
static std::condition_variable mutex_condition;

// Waiting loop for threads
static void threadloop(void){

    std::unique_lock<std::mutex> lock(queue_mutex);

    Atomic_cout() << "Thread: " << std::this_thread::get_id() << " is waiting" << std::endl;
    mutex_condition.wait(lock, []{return ready;});

    Atomic_cout() << "Thread: " << std::this_thread::get_id() << " is doing work" << std::endl;
    ready = false;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    lock.unlock();
}

void start(size_t num_threads){

    std::ostringstream string;

    for(int i = 0; i < num_threads; i++){
        threads.emplace_back(std::thread(&threadloop));
        Atomic_cout() << "thread " << threads[i].get_id() << " created" << std::endl;
    }

    while(1){
    Atomic_cout() << "waiting to continue..." << std::endl;
    std::cin.get();
    std::unique_lock<std::mutex> lock(queue_mutex);
    ready = true;
    mutex_condition.notify_one();
    lock.unlock();
    }

    std::cout << "done" << std::endl;
}
