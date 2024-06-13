#include "client_helper.h"

int open_client_fd(const char *hostname, int port) {
    int client_fd;
    
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){ // Create socket
        return -1; 
    }

    struct hostent *hp; // Struct used to get info about server
    // Get server's IP address 
    if ((hp = gethostbyname(hostname)) == NULL){
        return -2; // check h_errno for cause of error
    }

    struct sockaddr_in server_addr; // Struct used to connect to server
    bzero((char *) &server_addr, sizeof(server_addr)); // Zero memory
    server_addr.sin_family = AF_INET;

    // Copy server IP & port to sockaddr_in struct
    bcopy((char *) hp->h_addr, 
          (char *) &server_addr.sin_addr.s_addr, hp->h_length);
    server_addr.sin_port = htons(port); // Convert port number from host endianness to network
    
    // Establish a connection with the server 
    if (connect(client_fd, (sockaddr_t *) &server_addr, sizeof(server_addr)) < 0){
        return -1;
    }

    return client_fd;
}

void client_send(int fd, const std::string &filename){
    char hostname[MAXBUF];
    gethostname_or_die(hostname, MAXBUF);

    std::ostringstream request;
    request << "GET " << filename << " HTTP/1.1\r\n";
    request << "host: " << hostname << "\r\n\r\n";
    std::string request_str = request.str();
    send_or_die(fd, request_str.c_str(), request_str.length(), 0);
}

void client_recv(int fd){
    char buf[MAXBUF];
    ssize_t bytes_received;

    /* 
    Two seperate recv() calls are needed as two seperate send() calls 
    are used by the server (for headers and body respectively)
    */
    if((bytes_received = recv(fd, buf, MAXBUF-1, 0)) < 0){
        std::cerr << "recv1 failed" << std::endl;
        return;
    }
    buf[bytes_received] = '\0';
    std::string request = buf;

    if((bytes_received = recv(fd, buf, MAXBUF-1, 0)) < 0){
        std::cerr << "recv2 failed" << std::endl;
        return;
    }
    buf[bytes_received] = '\0';
    request = buf;
}