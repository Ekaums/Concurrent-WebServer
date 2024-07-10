#pragma once
int open_listen_fd(int port);

typedef struct sockaddr sockaddr_t;
typedef struct sockaddr_in sockaddr_in_t;

#define DEFAULT_PORT (20000)
#define MAX_EVENTS (100)
#define QUEUE_SIZE (1024)

#define chdir_or_die(path) \
  assert(chdir(path) == 0);

#define open_listen_fd_or_die(port) \
  ({ int rc = open_listen_fd(port); assert(rc >= 0); rc;})
  
#define accept_or_die(fd) \
  ({ int rc = accept(fd, NULL, NULL); assert(rc >= 0); rc;})

#define chdir_or_die(path) \
  assert(chdir(path) == 0);
