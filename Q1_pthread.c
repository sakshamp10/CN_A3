#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

#define PORT 4444
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

unsigned int min(unsigned int a, unsigned int b){
    return a < b ? a : b;
}

unsigned int fact(unsigned int n){
    unsigned int result = 1;
    for(unsigned int j = 1; j <= min(n, 20); j++){ // Limit to prevent overflow
        result *= j;
    }
    return result;
}

void *clientHandler(void *arg) {
    int clientSocket = *((int *)arg);
    free(arg);

    char buffer[BUFFER_SIZE + 1];
    int bytesRead;

    send(clientSocket, "hi client", strlen("hi client"), 0);

    while (1) {
        bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0) {
            if (bytesRead < 0) perror("recv error");
            break;
        }
        buffer[bytesRead] = '\0';

        unsigned int num = atoi(buffer);
        unsigned int ans = fact(num);

        char charArray[BUFFER_SIZE];
        snprintf(charArray, sizeof(charArray), "%u", ans);
        send(clientSocket, charArray, strlen(charArray), 0);
    }

    close(clientSocket);
    return NULL;
}

int main() {
    int sockfd, ret;
    struct sockaddr_in serverAddr;
    int clientSocket;
    struct sockaddr_in cliAddr;
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error in connection");
        exit(1);
    }
    printf("Server Socket is created\n");

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ret < 0) {
        perror("Error in binding");
        close(sockfd);
        exit(1);
    }

    if (listen(sockfd, MAX_CLIENTS) == 0) {
        printf("Listening...\n\n");
    } else {
        perror("listen error");
        close(sockfd);
        exit(1);
    }

    while (1) {
        addr_size = sizeof(cliAddr);
        clientSocket = accept(sockfd, (struct sockaddr *)&cliAddr, &addr_size);
        if (clientSocket < 0) {
            perror("accept error");
            continue;
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));

        int *clientSocketPtr = malloc(sizeof(int));
        if(clientSocketPtr == NULL) {
            perror("malloc error");
            close(clientSocket);
            continue;
        }
        *clientSocketPtr = clientSocket;

        pthread_t tid;
        if (pthread_create(&tid, NULL, clientHandler, (void *)clientSocketPtr) != 0) {
            perror("pthread_create error");
            free(clientSocketPtr);
            close(clientSocket);
            continue;
        }
        pthread_detach(tid);
    }

    close(sockfd);
    return 0;
}
