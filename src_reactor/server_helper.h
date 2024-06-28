#pragma once

// TODO: dont pile all headers here mayb
#include <iostream>
#include <assert.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>

typedef struct sockaddr sockaddr_t;
typedef struct sockaddr_in sockaddr_in_t;

int open_listen_fd(int port);

#define DEFAULT_PORT (20000)
#define QUEUE_SIZE (1024)
#define MAX_EVENTS (100)

#define chdir_or_die(path) \
    assert(chdir(path) == 0);

#define open_listen_fd_or_die(port) \
    ({ int rc = open_listen_fd(port); assert(rc >= 0); rc; })

#define accept_or_die(s, addr, addrlen) \
    ({ int rc = accept(s, addr, addrlen); assert(rc >= 0); rc; })

#define open_or_die(pathname, flags, mode) \
    ({ int rc = open(pathname, flags, mode); assert(rc >= 0); rc; })

#define close_or_die(fd) \
    assert(close(fd) == 0); 

#define send_or_die(fd, buf, count, flags) \
    ({ ssize_t rc = send(fd, buf, count, flags); assert(rc >= 0); rc; })

#define mmap_or_die(addr, len, prot, flags, fd, offset) \
    ({ void *ptr = mmap(addr, len, prot, flags, fd, offset); assert(ptr != (void *) -1); ptr; })

#define munmap_or_die(start, length) \
    assert(munmap(start, length) >= 0);

#define fork_or_die() \
    ({ pid_t pid = fork(); assert(pid >= 0); pid; })

#define setenv_or_die(name, value, overwrite) \
    ({ int rc = setenv(name, value, overwrite); assert(rc == 0); rc; })

#define dup2_or_die(fd1, fd2) \
    ({ int rc = dup2(fd1, fd2); assert(rc >= 0); rc; })

#define execve_or_die(filename, argv, envp) \
    assert(execve(filename, argv, envp) == 0);

#define wait_or_die(status) \
    ({ pid_t pid = wait(status); assert(pid >= 0); pid; })

#define chdir_or_die(path) \
    assert(chdir(path) == 0);

#define kqueue_or_die() \
     ({ int kq = kqueue(); assert(kq >= 0); kq; })