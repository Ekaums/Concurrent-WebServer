#include <stdio.h>
#include "request.h"
#include "io_helper.h"

char default_root[] = ".";

//
// ./wserver [-d <basedir>] [-p <portnum>] 
// 
int main(int argc, char *argv[]) {
    int c;
	// Default arguments
    char *root_dir = default_root;
    int port = 10000;
    
    while ((c = getopt(argc, argv, "d:p:")) != -1){ // Check if user provides alternative arguments
		switch (c) {
		case 'd':
			root_dir = optarg;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		default:
			fprintf(stderr, "usage: wserver [-d basedir] [-p port]\n");
			exit(1);
		}
	}

    // Change directory for website, or exit (die)
    chdir_or_die(root_dir);

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port); // Open a communication endpoint (to listen on)
    while (1) {
	struct sockaddr_in client_addr; // Why isnt this outside the for loop
	int client_len = sizeof(client_addr);

	// Accept next incoming connection
	// Note: In this setup will accept block until a client connects? is this fine?
	int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
								// 1. Listening socket descriptor (fd)
							   // 2. Struct that will be filled in with the info of the client addr.
							  // 3. Size
	
	// Handle it
	request_handle(conn_fd);

	// Must close() once done with request
	close_or_die(conn_fd);
    }
    return 0;
}


    


 
