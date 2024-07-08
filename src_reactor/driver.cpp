#include "server_helper.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <assert.h>

#define chdir_or_die(path) \
  assert(chdir(path) == 0);

#define open_listen_fd_or_die(port) \
  ({ int rc = open_listen_fd(port); assert(rc >= 0); rc; })

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

}
