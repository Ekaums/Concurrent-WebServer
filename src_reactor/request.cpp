#include "request.h"
#include <sstream>
#include <iostream>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/wait.h>

static void request_get_filetype(const std::string &filename, std::string &filetype) {
  if (filename.find(".html") != std::string::npos) 
    filetype = "text/html";
  else if (filename.find(".gif") != std::string::npos) 
    filetype = "image/gif";
  else if (filename.find(".jpg") != std::string::npos) 
    filetype ="image/jpeg";
  else 
    filetype = "text/plain";
}

static void request_serve_static(int fd, const std::string &filename, int filesize){
  std::string filetype;
  request_get_filetype(filename, filetype);

  int srcfd = open_or_die(filename.c_str(), O_RDONLY, 0);

  void *srcp = mmap_or_die(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
  close_or_die(srcfd);

  std::ostringstream response;
  response  << "HTTP/1.0 200 OK\r\n"
            << "Server: WebServer\r\n"
            << "Content-Length: " << filesize << "\r\n"
            << "Content-Type: " << filetype << "\r\n\r\n";

  std::string response_str = response.str();
  send_or_die(fd, response_str.c_str(), response_str.length(), 0);

  send_or_die(fd, (char *)srcp, filesize, 0);
  munmap_or_die(srcp, filesize);
}

static void request_serve_dynamic(int fd, const std::string &filename, const std::string &cgiargs){
  char buf[READ_SIZE] = "HTTP/1.0 200 OK\r\nServer: WebServer\r\n";
  send_or_die(fd, buf, strlen(buf), 0);

  char *argv[] = {NULL};

  if (fork_or_die() == 0) {                                // child
    setenv_or_die("QUERY_STRING", cgiargs.c_str(), 1);  // args to cgi go here
    dup2_or_die(fd, STDOUT_FILENO);                     // make cgi writes go to socket (not screen)
    extern char **environ;                              // defined by libc. Passed to provide all other environment variables
    execve_or_die(filename.c_str(), argv, environ);     // execute program, with no command line args (argv)
  } 
  else {
    wait_or_die(NULL);
  }
}


static void request_error(int fd, const std::string &errnum, const std::string &shortmsg, const std::string &longmsg, const std::string &cause){
  std::ostringstream body, response;
  std::string body_str, response_str;

  body  << "<!doctype html>\r\n"
        << "<head>\r\n"
        << "  <title>WebServer Error</title>\r\n"
        << "</head>\r\n"
        << "<body>\r\n"
        << "  <h2>" << errnum << ": " << shortmsg << "</h2>\r\n"
        << "  <p>" << longmsg << ": " << cause << "</p>\r\n"
        << "</body>\r\n"
        << "</html\r\n";

  body_str = body.str();

  response  << "HTTP/1.0 " << errnum << " " << shortmsg << "\r\n"
            << "Content-Type: text/html\r\n"
            << "Content-Length: " << body_str.length() << "\r\n\r\n";
  
  response_str = response.str();
  
  send_or_die(fd, response_str.c_str(), response_str.length(), 0);
  send_or_die(fd, body_str.c_str(), body_str.length(), 0);
}


static bool request_parse_uri(const std::string &uri, std::string &filename, std::string &cgiargs){
  if(uri.find("cgi") == std::string::npos){
    // static
    cgiargs = "";
    filename = "." + uri;
    if(uri.back() == '/'){
      filename += "index.html";
    }
    return true;
  }
  else{
    // dynamic
    size_t args = uri.find("?");
    if(args == std::string::npos){
      cgiargs = "";
    }
    else{
      cgiargs = uri.substr(args+1);
    }
    filename = "." + uri;
    return false;
  }
}

void handle_request(int fd){
  char buf[READ_SIZE];
  ssize_t bytes_received;

  // MSG_PEEK flag is useful to peek at the type of data!
  // MSG_DONTWAIT for non-blocking
  if((bytes_received = recv(fd, buf, READ_SIZE-1, 0)) <= 0){
    std::cerr << "recv failed" << std::endl;
    return;
  }

  buf[bytes_received] = '\0';

  std::istringstream request_stream((std::string)buf);

  // parse request line
  std::string method, uri, version;
  request_stream >> method >> uri >> version;
  std::cout << "Method: " << method << " URI: " << uri << " version: " << version << std::endl;

  if(method != "GET"){
    std::cout << "method is not GET" << std::endl;
    request_error(fd, "501", "Not implemented", "this method is not implemented", method);
    return;
  }

  std::string filename, cgiargs;
  bool is_static = request_parse_uri(uri, filename, cgiargs);
  struct stat sbuf;
  if(stat(filename.c_str(), &sbuf) < 0){
    request_error(fd, "404", "Not found", "could not find this file", filename);
    return;
  }

  if(is_static){
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)){
    // Check if filename is a file (not a directory, symlink, etc.) and that it can be read
      request_error(fd, "403", "Forbidden", "could not read this file", filename);
      return;
    }
    request_serve_static(fd, filename, sbuf.st_size);
  }
  else{
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)){
    // Check if filename is a file and that it can be executed
      request_error(fd, "403", "Forbidden", "could not run this CGI program", filename);
      return;
    }
    request_serve_dynamic(fd, filename, cgiargs);
  }
}

