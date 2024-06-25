#pragma once

// TODO: dont pile all headers here mayb
#include <assert.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>

typedef struct sockaddr sockaddr_t;
typedef struct sockaddr_in sockaddr_in_t;

int open_listen_fd(int port);

#define DEFAULT_PORT (10000)
#define QUEUE_SIZE (1024)
#define MAX_EVENTS (100)

#define chdir_or_die(path) \
    assert(chdir(path) == 0);

#define open_listen_fd_or_die(port) \
    ({ int rc = open_listen_fd(port); assert(rc >= 0); rc; })

#define chdir_or_die(path) \
    assert(chdir(path) == 0);

#define kqueue_or_die() \
     ({ int kq = kqueue(); assert(kq >= 0); kq; })