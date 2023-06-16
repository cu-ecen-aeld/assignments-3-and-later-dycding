#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#define O_FILE "/var/tmp/aesdsocketdata"
#define B_SIZE 1024

int socket_listen, socket_client;
int op = 1;

void sigHandler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
	syslog(LOG_INFO, "Caught signal, exiting\n");
	close(socket_listen);
	close(socket_client);
	remove(O_FILE);
	exit(1);
    }
}

int main(int argc, char** argv) {
    // Open system log
    openlog("aesdsocket", 0, LOG_USER);

    // Check whether the application is run in the daemon mode
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        daemon(0, 0);
    }

    // Handling signals
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

    // Figure out the local address
    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *bind_address;
    int status = getaddrinfo(0, "9000", &hints, &bind_address);
    if (status < 0) {
        syslog(LOG_ERR, "Address configuration failed. (%d)\n", errno);
        return -1;
    }

    // Create the socket
    printf("Creating socket...\n");
    socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    if (socket_listen < 0) {
        syslog(LOG_ERR, "socket() failed. (%d)\n", errno);
        return -1;
    }

    setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));

    // Call bind() to associate the socket with the address
    printf("Binding socket to local address...\n");
    if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen)) {
        syslog(LOG_ERR, "bind() failed. (%d)\n", errno);
        return -1;
    }
    freeaddrinfo(bind_address);

    // Start listening for connections
    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0) {
        syslog(LOG_ERR, "listen() failed. (%d)\n", errno);
        return -1;
    }

    // Accept incoming connections
    while (1) {
	printf("Waiting for connection...\n");
        struct sockaddr_storage client_address;
        socklen_t client_len = sizeof(client_address);
        char addr_buffer[INET6_ADDRSTRLEN];
        int socket_client = accept(socket_listen, (struct sockaddr*) &client_address, &client_len);
        if (socket_client < 0) {
            syslog(LOG_ERR, "accept() failed. (%d)\n", errno);
            return -1;
        }
        else if (client_address.ss_family == AF_INET) {
            struct sockaddr_in *addr = (struct sockaddr_in *) &client_address;
            inet_ntop(AF_INET, addr, addr_buffer, sizeof(addr_buffer));
        }
        else if (client_address.ss_family == AF_INET6) {
            struct sockaddr_in6 *addr = (struct sockaddr_in6 *) &client_address;
            inet_ntop(AF_INET, addr, addr_buffer, sizeof(addr_buffer));
        }
        else {
            fprintf(stderr, "Unresolved address type.\n");
        }
        syslog(LOG_INFO, "Accepted connection from %s.\n", addr_buffer);

        // Prepare files to write (append) the received data
        int out_file = open(O_FILE, O_RDWR | O_APPEND | O_CREAT, 0666);
        if (out_file < 0) {
            syslog(LOG_ERR, "open() error: %d\n", errno);
            return -1;
	}

        // Read the HTTP request
	printf("Reading request...\n");
	char request[B_SIZE];
	int bytes_received;
	while ((bytes_received = recv(socket_client, request, B_SIZE-1, 0)) > 0) {
	    printf("Received %d bytes.\n", bytes_received);
	    write(out_file, request, bytes_received);
	    if (request[bytes_received-1] == '\n') {
	        break;
	    }
	}

	// Send data
	int bytes_read;
	lseek(out_file, 0, SEEK_SET);
	while ((bytes_read = read(out_file, request, B_SIZE)) > 0) {
	    send(socket_client, request, bytes_read, 0);
	}

        close(out_file);
	shutdown(socket_client, SHUT_RDWR);
        close(socket_client);
	syslog(LOG_INFO, "Closed connection from %s.\n", addr_buffer);
    }

    shutdown(socket_listen, SHUT_RDWR);
    close(socket_listen);
    freeaddrinfo(bind_address);

    return 0;
}
