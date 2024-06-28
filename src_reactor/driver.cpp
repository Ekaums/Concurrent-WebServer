#include <string>
#include "server_helper.h"
#include "request.h"

/*
  ./server [-d <basedir>] [-p <port>]
*/
int main(int argc, char *argv[]){

  // default args
  std::string root_dir = ".";
  int port = DEFAULT_PORT;

  int c;
  // check if user provides alternative arguments
  while ((c = getopt(argc, argv, "d:p:")) != -1){
    switch(c){
        case 'd':
        root_dir = optarg;
        std::cerr << "changed directory to " << root_dir << std::endl;
        break;

        case 'p':
        port = atoi(optarg);
        std::cerr << "changed port to " << port << std::endl;
        break;

        default:
        std::cerr << "usage: ./server [-d basedir] [-p port]" << std::endl;
        exit(1);
    }
  }

  // change directory for website, or exit (die)
  chdir_or_die(root_dir.c_str());

  // listen on port (using fd)
  int listenfd = open_listen_fd_or_die(port);

  /*  kqueue init  */
  int kq = kqueue_or_die();
  
  struct kevent event;

  // macro to init struct event
  // Init listen socket for monitoring on read-ready events
  EV_SET(&event, listenfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
  /*
    EVFILT_READ = Monitor for read availibility
    EV_ADD = Add to event queue
    EV_ENABLE = Enable event
  */

  // update kqueue with this event
  if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
      std::cerr << "kevent" << std::endl;
      exit(1);
  }

  struct kevent events[MAX_EVENTS];
  sockaddr_in_t client_addr;
  int client_len = sizeof(client_addr);

  // get to work
  std::cout << "ready" << std::endl;
  while(true){
    
    // Retrieve new events
    int nev = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
    if(nev == -1){
        std::cerr << "kevent" << std::endl;
        exit(1);
    }

    // Handle them
    for(int i = 0; i < nev; i++){

        // If there is data to read from the socket
        if(events[i].filter == EVFILT_READ){

            // And it is the server socket -- incoming connection
            if(events[i].ident == listenfd){
                std::cout << "accepting new connection" << std::endl;
                int connfd = accept_or_die(listenfd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);

                // add new client to kqueue in order to monitor
                EV_SET(&event, connfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
                    std::cerr << "kevent" << std::endl;
                    // do we want to exit here? or simply not handle this
                    exit(1);
                }
            }
            // Not server socket -- incoming request from existing client
            else{
                std::cout << "handling request" << std::endl;
                int clientfd = events[i].ident;
                handle_request(clientfd);
                EV_SET(&event, clientfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
                  std::cerr << "kevent EV_DELETE" << std::endl;
                }
                close(clientfd);
            }
        }  
    }
  }

  close(kq);
}

// TODO: FIX FORMATTING of tabs
// TODO: clean up main loop by creating functions