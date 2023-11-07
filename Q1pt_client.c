#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h> // Include the pthread library

// PORT number
#define PORT 4444

// Function to handle server communication in a separate thread
void *serverHandler(void *arg) {
    int clientSocket = *((int *)arg);
    free(arg); // Free the memory allocated for the argument

    char buffer[1024];
    while (1) {
        // recv() receives the message from the server and stores it in the buffer
        if (recv(clientSocket, buffer, 1024, 0) < 0) {
            printf("Error in receiving data.\n");
            break; // Exit the thread on error
        } else {
            printf("Server: %s\n", buffer);
            bzero(buffer, sizeof(buffer));
        }
    }

    close(clientSocket); // Close the client socket
    return NULL;
}

int main() {
    // Socket id
    int clientSocket, ret;

    // Client socket structure
    struct sockaddr_in serverAddr;

    // char array to store incoming message
    char buffer[1024];

    // Creating socket id
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        printf("Error in connection.\n");
        exit(1);
    }
    printf("Client Socket is created.\n");

    // Initializing socket structure with NULL
    memset(&serverAddr, 0, sizeof(serverAddr));

    // Initializing buffer array with NULL
    memset(buffer, 0, sizeof(buffer));

    // Assigning port number and IP address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    // 127.0.0.1 is Loopback IP
    serverAddr.sin_addr.s_addr = inet_addr("10.0.2.15");

    // connect() to connect to the server
    ret = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    if (ret < 0) {
        printf("Error in connection.\n");
        exit(1);
    }

    printf("Connected to Server.\n");

    // Create a thread to handle server communication
    int *clientSocketPtr = (int *)malloc(sizeof(int));
    *clientSocketPtr = clientSocket;
    pthread_t tid;
    pthread_create(&tid, NULL, serverHandler, (void *)clientSocketPtr);
    pthread_detach(tid); // Detach the thread to clean up resources

    // User input and send messages to the server
    while (1) {
        printf("Client: ");
        fgets(buffer, sizeof(buffer), stdin);
        send(clientSocket, buffer, strlen(buffer), 0);
        bzero(buffer, sizeof(buffer));
    }

    return 0;
}
