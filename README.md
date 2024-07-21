# Concurrent Webserver in C++
This project explored and implemented multiple web server architectures in C++. The goal was to optimize the server's design to be as fast as possible under a high workload, utilizing approaches such as a threadpool, I/O multiplexing with epoll(), and non-blocking sockets. Currently, the server implements GET requests, capable of sending html, css, javscripts, images. More functionality is to be added at a later date.
 
# Server Designs
3 Different designs were explored, each increasing the server's performance.

## Threadpool
The threadpool design utilizes a pool of pre-created threads to balance the workload of incoming requests. The thread pool follows the *producer-consumer* pattern, which maintains one producer thread and multiple consumer threads with a shared queue.

-  The producer thread accepts a new connections, and inserts the request into the queue. This is done by the event loop in the main thread.
- The consumer threads are responsible for taking a connection from the shared queue, processing the request and sending the response. After this is complete, the consumer will wait to take a new connection from the queue.
- As the queue is shared and accessed by multiple threads, it needs to be protected and synchronized. To achieve this, the queue uses a `std::mutex` lock and two `std::condition_variable`.
	- The `std::mutex` lock is used to protect the queue from concurrent access. It is important to note that `std::queue` is not thread-safe therefore at any time, only one thread, either consumer or producer, can access the queue. 
	- The `empty` semaphore is used by the consumers to signal the producer thread that the queue has empty slots and the main thread can therefore run. When the queue is full, this semaphore will make the producer thread blocked.
	- The `full` semaphore is used by the producer thread to signal the consumer threads that the queue has a pending task. When the queue is empty, this semaphore will make the consumer threads blocked.

## Epoll
The epoll design utilizes uses the `epoll()` syscall to monitor multiple sockets in a single-threaded design. This approach uses event-based concurrency, where the main event loop monitors all client connections (and the server socket), waiting to see if any I/O is possible on them. This allows the server to handle multiple connections without blocking. Although this design may seem inefficient by using a single thread, it has the advantage of simplicity. A single-threaded design does not bring the overhead of multiple threads, as well as the performance issues of constantly blocking and context-swapping threads.

This design can be further optimized with non-blocking sockets. This means that system calls like `recv()` and `send()` will read or write however much as they can, and return immediately. 

## Reactor
The ultimate design will be a hybrid of both designs and in theory has the best performance. Although this approach was not implemented, in theory it takes the best aspects of both architectures and combines them. But what makes this a hybrid approach?

The way that the epoll approach handles I/O (with non-blocking sockets) is more efficient, as the server does not waste time waiting to completely receive/send a request. Instead, it does as much I/O as it can on a connection, then comes back later when it can handle more. This is great, however, the design is single-threaded and therefore does not completely utilize the CPU. The CPU-intensive aspect of the server, that is, processing and forming a request, is only being done on a single thread!

The threadpool is best for utilizing all CPU cores of a system. I/O requests are not CPU-intensive and therefore can be left with the single-thread loop. But processing these requests IS CPU-intensive and is therefore best done with multiple threads (a threadpool).

This design is an adaptation of the [Reactor Pattern](https://en.wikipedia.org/wiki/Reactor_pattern).  The reactor pattern is a server architecture that utilizes an event loop to listen to events (like with `epoll`) and dispatches work to handlers. Handlers can be other processes or, in this case, other threads.


# Requirements
There aren't a lot of requirements to run. Just ensure that cmake is installed and you are using Linux.

# Benchmarking
The following benchmarks were performed using the [wrk](https://github.com/wg/wrk) tool, a popular HTTP benchmarking tool.

### Threadpool
```
Running 20s test @ http://localhost:10000
  8 threads and 10000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    78.96ms  126.97ms   1.82s    94.95%
    Req/Sec     1.98k     1.58k    8.38k    77.88%
  92501 requests in 20.10s, 148.82MB read
  Socket errors: connect 0, read 270, write 0, timeout 1170
Requests/sec:   4601.36
Transfer/sec:      7.40MB
```

### Epoll
```
Running 20s test @ http://localhost:10000
  8 threads and 10000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    71.94ms  195.74ms   1.99s    94.37%
    Req/Sec     4.33k     1.92k   11.36k    69.50%
  680832 requests in 20.09s, 1.07GB read
  Socket errors: connect 0, read 0, write 0, timeout 1677
Requests/sec:  33886.30
Transfer/sec:     54.52MB
```

As can be seen, the epoll design handled more requests total as well as requests/second. More specifically, there was about a **7.3x** increase in request throughput.  I had assumed this would be the result, as the event-loop provides less overhead with constant thread-blocking and context switching. A great explanation can also be found [here](https://stackoverflow.com/a/2902718).