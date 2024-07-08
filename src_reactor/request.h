#pragma once

#define MAXBUF (8192)

void handle_request(int fd);

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
