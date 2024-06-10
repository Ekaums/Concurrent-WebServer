#include "server_helper.h"
#include "request.h"
#include "threadpool.h"

/*
  ./server [-d <basedir>] [-p <port>] [-t <num_threads>] [-b <buffer_size>]
*/
int main(int argc, char *argv[]){

  // default args
  std::string root_dir = ".";
  int port = DEFAULT_PORT;
  size_t threads = 1;
  size_t buffer = 3;

  int c;
  // check if user provides alternative arguments
  while ((c = getopt(argc, argv, "d:p:t:b:")) != -1){
    switch(c){
      case 'd':
      root_dir = optarg;
      std::cerr << "changed directory to " << root_dir << std::endl;
      break;

      case 'p':
      port = atoi(optarg);
      std::cerr << "changed port to " << port << std::endl;
      break;

      case 't':
      threads = strtoul(optarg, NULL, 10);
      std::cerr << "using " << threads << " threads" << std::endl;
      break;

      case 'b':
      buffer = strtoul(optarg, NULL, 10);
      std::cerr << "using buffer size " << buffer << std::endl;
      break;

      default:
      std::cerr << "usage: ./server [-d basedir] [-p port] [-t <num_threads>] [-b <buffer_size>]" << std::endl;
      exit(1);
    }
  }

  // change directory for website, or exit (die)
  chdir_or_die(root_dir.c_str());

  // listen on fd
  int fd = open_listen_fd_or_die(port);
  
	sockaddr_in_t client_addr;
	int client_len = sizeof(client_addr);

  // Create threadpool
  Threadpool deadpool(threads, buffer);

  // get to work
  while(true){
    // accept next incoming connection
    int conn_fd = accept_or_die(fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
    std::cout << "accepted connection" << std::endl;

    deadpool.queueJob(conn_fd);
  }

}