#include "client_threadpool.h"
#include "client_helper.h"

Threadpool::Threadpool(size_t num_threads, std::string server_addr, int server_port, std::string file) : port(server_port), hostname(server_addr), filename(file)
{
    for(size_t i = 0; i < num_threads; i++){
        threads.emplace_back(std::thread(&Threadpool::sendRequest, this));
    }
}

void Threadpool::sendRequest(void){
    while(true){
    /* Slight delay to prevent resource exhaustion.
       Rapidly opening/closing fds can exhaust epheremal ports
    */
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    /* Open connection to the server*/
    std::cout << "opening conn...\n";
    int clientfd = open_client_fd_or_die(hostname.c_str(), port);
    std::cout << "opened conn\n";

    /* Send request */
    std::cout << "sending rq...\n";
    client_send(clientfd, filename);
    std::cout << "sent rq\n";

    /* Print response*/
    std::cout << "getting response...\n";
    client_recv(clientfd);
    std::cout << "got response\n";

    std::cout << "closing...\n";
    close_or_die(clientfd);
    std::cout << "closed\n";
    }
}