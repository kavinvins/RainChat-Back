/**
  @file  server.c
  @brief Instant messaging API
*/

#include "server.h"

int main(int argc, char *argv[]) {
    int sockfd;
    sockfd = initSocket(argv[1], argv[2]);
    initClient(&sockfd);
    pthread_exit(NULL);
}

void initClient(int *sockfd) {
    int state;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    pthread_t *thread_id;
    User *user = malloc(sizeof(User));
    tail = head;

    for (int i=0; i<10; i++) {
        // accept incoming request, create new client socket
        user->socket = accept(*sockfd, (struct sockaddr *) &cli_addr, &clilen);
        thread_id = malloc(sizeof(pthread_t));
        checkError(user->socket,
                   "ERROR on accepting",
                   "Accepted");

        state = pthread_create(thread_id, NULL, initRecvSession, (void *)user);
        if (state){
            printf("ERROR; return code from pthread_create() is %d\n", state);
            exit(-1);
        }

        // should test if successfully create a user
        user = malloc(sizeof(User));

    }

    // close socket
    // printf("sockfd closed\n");
    // close(*sockfd);
}

void *initRecvSession(void *param_user) {
    User *user = (User*)param_user;
    char *message;
    http_frame frame;

    // assign temporary username
    user->name = calloc(20, sizeof(char));
    strcpy(user->name, "kavinvin");

    // prepend user to the linked list
    head = prepend(user, head);

    checkError(open_handshake(user->socket),
               "handshaking failed",
               "handshaking succeed");

    while (1) {
        // receive message from client
        memset(&frame, 0, sizeof(frame));
        ws_recv(user->socket, &frame);

        message = frame.message;
        parseMessage(user->socket, message);

        // send message to client
        memset(&frame, 0, sizeof(frame));
        frame.opcode = 129;
        frame.message = message;
        frame.size = strlen(frame.message);
        // ws_send(user->socket, &frame);

        map(head, broadcast, &frame);

    }

    close(user->socket);
    printf("newsockfd no.%d closed\n", user->socket);
    pthread_exit(NULL);
}

int parseMessage(int sockfd, char *message) {
    if (*message == '/') {
        // command mode
        getCommand(message);
        return 1;
    } else {
        // message mode
        printf("Here is the message from no.%d: %s\n", sockfd, message);
        return 0;
    }
}

void getCommand(char *command) {
    if (strcmp(command, "/exit") == 0) {
        printf("Exited\n");
        pthread_exit(NULL);
    } else {
        printf("Command not found\n");
    }
}
