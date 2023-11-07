#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 4444

int clientSocket;

void *clientThread(void *arg) {
    char buffer[1024];

    while (1) {
        if (recv(clientSocket, buffer, 1024, 0) < 0) {
            printf("Error in receiving data.\n");
        } else {
            printf("Server: %s\n", buffer);
            bzero(buffer, sizeof(buffer));
        }
    }

    return NULL;
}

int main() {
    struct sockaddr_in serverAddr;
    pthread_t tid;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        printf("Error in connection.\n");
        exit(1);
    }
    printf("Client Socket is created.\n");

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.2.15");

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr) < 0) {
        printf("Error in connection.\n");
        exit(1);
    }
    printf("Connected to Server.\n");

    pthread_create(&tid, NULL, clientThread, NULL);

    while (1) {
        // Add your logic here for sending data from client to server if needed

        char message[1024];
        fgets(message, 1024, stdin);
        send(clientSocket, message, strlen(message), 0);
    }

    pthread_join(tid, NULL);
    close(clientSocket);

    return 0;
}
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 4444

int clientSocket;

void *clientThread(void *arg) {
    char buffer[1024];

    while (1) {
        if (recv(clientSocket, buffer, 1024, 0) < 0) {
            printf("Error in receiving data.\n");
        } else {
            printf("Server: %s\n", buffer);
            bzero(buffer, sizeof(buffer));
        }
    }

    return NULL;
}

int main() {
    struct sockaddr_in serverAddr;
    pthread_t tid;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        printf("Error in connection.\n");
        exit(1);
    }
    printf("Client Socket is created.\n");

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.2.15");

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr) < 0) {
        printf("Error in connection.\n");
        exit(1);
    }
    printf("Connected to Server.\n");

    pthread_create(&tid, NULL, clientThread, NULL);

    while (1) {
        // Add your logic here for sending data from client to server if needed

        char message[1024];
        fgets(message, 1024, stdin);
        send(clientSocket, message, strlen(message), 0);
    }

    pthread_join(tid, NULL);
    close(clientSocket);

    return 0;
}
