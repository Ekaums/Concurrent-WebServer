#include <string>
#include "server_helper.h"

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

  // listen on fd
  int fd = open_listen_fd_or_die(port);

  /*  kqueue init  */
  int kq = kqueue_or_die();
  
  struct kevent event;

  // macro to init struct event
  // Init listen socket for monitoring on read-ready events
  EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
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

  while(1){
    
    // Retrieve new events
    int nev = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
    if(nev == -1){
      std::cerr << "kevent" << std::endl;
      exit(1);
    }

    // Handle them
    for(int i = 0; i < nev; i++){
      if(events[i].filter == EVFILT_READ){
        if(events[i].ident == fd){
          std::cout << "new connection incoming" << std::endl;
        }
      }
    }
  }


}