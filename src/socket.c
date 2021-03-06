/**
 * File: socket.c
 * ----------------------------
 *   function of raw socket communication
 */

#include "socket.h"

/**
 * Function: initSocket
 * ----------------------------
 *   initSocket(host, port)
 *   create server socket
 *   return socket descriptor, -1 if failed to create socket
 */
int initSocket(char *host, char *port) {
    int sockfd, option=1;
    struct sockaddr_in server_address;

    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    printlog("-- Creating socket --\n");
    if (sockfd < 0) {
        printlog("ERROR on opening socket: %s\n", strerror(errno));
        return -1;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // create server address structure
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(port)); // htons converts from host byte order to network byte order
    server_address.sin_addr.s_addr = inet_addr(host); // inet_addr convert dot notation into network address in Network Byte Order

    // bind the socket to the server address
    printlog("-- Binding --\n");
    if (bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        printlog("ERROR on binding: %s\n", strerror(errno));
        return -1;
    }

    // listening to the socket
    printlog("-- Listening --\n");
    if (listen(sockfd, 5)) {
        printlog("ERROR on listening: %s\n", strerror(errno));
        return -1;
    }

    return sockfd;
}
