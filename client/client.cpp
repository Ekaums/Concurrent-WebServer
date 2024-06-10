#include "client_helper.h"

#define DEFAULT_PORT (10000)

/*
  ./client [-h host] [-p port] [-f <filename>] [-t <num_threads>]
*/
int main(int argc, char *argv[]) {

  // default args
  std::string host = "localhost";
  int port = DEFAULT_PORT;
  std::string filename = "/";
  size_t threads = 1;

  int c;
  // check if user provides alternative arguments
  while ((c = getopt(argc, argv, "h:p:f:t:")) != -1){
    switch(c){
        case 'h':
        host = optarg;
        std::cerr << "using host " << host << std::endl;
        break;

        case 'p':
        port = atoi(optarg);
        std::cerr << "changed port to " << port << std::endl;
        break;

        case 'f':
        filename = optarg;
        std::cerr << "going to access file " << filename << std::endl;
        break;

        case 't':
        threads = strtoul(optarg, NULL, 10);
        std::cerr << "using " << threads << " threads" << std::endl;
        break;

        default:
        std::cerr << "usage: ./client [-h host] [-p port] [-f <filename>] [-t <num_threads>]" << std::endl;
        exit(1);
    }
  }

  /* Open connection to the server*/
  int clientfd = open_client_fd_or_die(host.c_str(), port);

  /* Send request */
  client_send(clientfd, filename);

  /* Print response*/
  client_recv(clientfd);

  close_or_die(clientfd);
}