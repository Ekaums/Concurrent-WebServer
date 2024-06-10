#include <string>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>
#include <sstream>

typedef struct sockaddr sockaddr_t;
#define MAXBUF (16384)

/* Establish connection w/ server*/
int open_client_fd(const char *hostname, int port);

/* Send request to server */
void client_send(int fd, const std::string &filename);

/* Get the HTTP response and poop it out */
void client_recv(int fd);

#define open_client_fd_or_die(hostname, port) \
    ({ int rc = open_client_fd(hostname, port); assert(rc >= 0); rc; })

#define gethostname_or_die(name, len) \
    ({ int rc = gethostname(name, len); assert(rc == 0); rc; })

#define send_or_die(fd, buf, count, flags) \
    ({ ssize_t rc = send(fd, buf, count, flags); assert(rc >= 0); rc; })

#define close_or_die(fd) \
    assert(close(fd) == 0); 

