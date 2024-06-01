#include "helper.h"
#include "request.h"

/*
  ./server [-d <basedir>] [-p <portnum>]
*/
int main(int argc, char *argv[]){

  // default arguments
  std::string root_dir = ".";
  int port = DEFAULT_PORT;

  int c;
  // check if user provides alternative arguments
  while ((c = getopt(argc, argv, "d:p:")) != -1){
    switch(c){
      case 'd':
      std::cerr << "changing directory to " << optarg << std::endl;
      root_dir = optarg;
      break;

      case 'p':
      std::cerr << "changing port to " << optarg << std::endl;
      port = atoi(optarg);
      break;

      default:
      std::cerr << "usage: wserver [-d basedir] [-p port]" << std::endl;
      exit(1);
    }
  }

  // change directory for website, or exit (die)
  chdir_or_die(root_dir.c_str());

  // listen on fd
  int fd = open_listen_fd_or_die(port);
  
	sockaddr_in_t client_addr;
	int client_len = sizeof(client_addr);

  // get to work
  while(true){
    // accept next incoming connection
    int conn_fd = accept_or_die(fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
    std::cout << "accepted connection" << std::endl;

    // handle it
    handle_request(conn_fd);

    // close it
    close_or_die(conn_fd);
  }

}