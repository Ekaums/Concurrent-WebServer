#include <string>
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include "include/server_helper.h"
#include "include/request.h"


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

  chdir_or_die(root_dir.c_str());

  // listen on port
  int listenfd = open_listen_fd_or_die(port);

  // epoll event loop stuff
  int epollfd;
  if((epollfd = epoll_create1(0)) == -1){
    std::cerr << "epoll failed" << std::endl;
    exit(1);
  }

  // For setting up events
  struct epoll_event event;

  // Monitor server socket 
  event.data.fd = listenfd;
  event.events = EPOLLIN;
  if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event) == -1){
    std::cerr << "epollctl failed" << std::endl;
    exit(1);
  }
  
  int nev, clientfd;
  struct epoll_event events[MAX_EVENTS]; // holds all new events returned by epoll

  std::cout << "starting event loop" << std::endl;
  while(true){

    // Retrieve new events
    nev = epoll_wait(epollfd, events, MAX_EVENTS, -1); // do we want a timeout??
    if(nev == -1){
      std::cerr << "epoll wait failed" << std::endl;
      exit(1);
    }

    // Handle them
    for(int i=0; i<nev; i++){

      if(events[i].events & EPOLLIN){ //Incoming data
        
        if(events[i].data.fd == listenfd){ // Incoming client connection
          std::cout << "accepting new conn" << std::endl;
          clientfd = accept_or_die(listenfd);

          // Add client to epoll buffer
          event.data.fd = clientfd;
          event.events = EPOLLIN;
          if(epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &event) == -1){
            std::cerr << "epollctl failed" << std::endl;
            exit(1);
          }
        }
        else{ // Existing client request
          std::cout << "client rq" << std::endl;
          clientfd = events[i].data.fd;
          handle_request(clientfd);
          close(clientfd);
        }
      }
      else{
        /* If doing partial R/W:
            First try sending as much data as possible.
            If we are able to send everything, done.
            If not, keep track of remaining data, subscribe client to EPOLLOUT, wait for signal, and send remaining data
        */
          
      }
    } 
  }
}
