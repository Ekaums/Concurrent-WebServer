#include "include/server_helper.h"

// Set up a socket to listen for incoming connections
int open_listen_fd(int port){

  int listen_fd;
  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    std::cerr << "socket failed" << std::endl;
    return -1;
  } 

  // Eliminates "Address already in use" error from bind
  int optval = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int)) < 0) {
    std::cerr << "setsockopt failed" << std::endl;
    return -1;
  }

  // Setting up a socket struct shenanigans
  sockaddr_in_t server_addr;
  bzero((char *) &server_addr, sizeof(server_addr)); // Zero memory
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
  server_addr.sin_port = htons((unsigned short) port);
  // server_addr is loaded up with IP address and port to bind to

  if (bind(listen_fd, (sockaddr_t *) &server_addr, sizeof(server_addr)) < 0){
    std::cerr << "bind failed" << std::endl;
    return -1;
  }

  // Listen for incoming connections
  if (listen(listen_fd, QUEUE_SIZE) < 0) {
    std::cerr << "listen failed" << std::endl;
    return -1;
  }

  return listen_fd;
}