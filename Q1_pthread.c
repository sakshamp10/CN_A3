#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

// PORT number
#define PORT 4444

unsigned int min(unsigned int a, unsigned int b){
    if(a<b) return a;
    return b;
}

unsigned int fact(unsigned int n){
    unsigned int i=1;
    for(unsigned int j=2;j<=min(n,20);j++){
        i=i*j;
    }
    return i;
}
// Function to handle client communication in a separate thread
void *clientHandler(void *arg) {
    int clientSocket = *((int *)arg);
    free(arg); // Free the memory allocated for the argument

    // Send a confirmation message to the client
    send(clientSocket, "hi client", strlen("hi client"), 0);

    char buffer[1024];
    int bytesRead;

    while (1) {
        // Receive the message from the client
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            break; // Exit the loop when the client disconnects
        } else {
            buffer[bytesRead] = '\0'; // Null-terminate the received data
            unsigned int result;

            result = atoi(buffer);
            printf("\n%u\n",result);
            unsigned int ans=fact(result);
            unsigned int ans2=ans,l=0;
            while(ans2>0){
                ans2=ans2/10;
                l++;
            }
            char charArray[l+1];
            sprintf(charArray, "%u", ans);
            charArray[l]='\0';
            send(clientSocket, charArray, strlen(charArray), 0);
            printf("Client: %s\n", charArray);
        }
    }

    // Close the client socket
    close(clientSocket);
    return NULL;
}

int main() {
    // Server socket id
    int sockfd, ret;

    // Server socket address structures
    struct sockaddr_in serverAddr;

    // Client socket id
    int clientSocket;

    // Client socket address structures
    struct sockaddr_in cliAddr;

    // Stores byte size of server socket address
    socklen_t addr_size;

    // Create a TCP socket id from IPV4 family
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Error handling if socket id is not valid
    if (sockfd < 0) {
        printf("Error in connection.\n");
        exit(1);
    }

    printf("Server Socket is created.\n");

    // Initialize the address structure with NULL
    memset(&serverAddr, 0, sizeof(serverAddr));

    // Assign port number and IP address to the socket created
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Use INADDR_ANY to bind to all available network interfaces

    // Bind the socket id with the socket structure
    ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // Error handling
    if (ret < 0) {
        printf("Error in binding.\n");
        exit(1);
    }

    // Listening for connections (up to 10)
    if (listen(sockfd, 10) == 0) {
        printf("Listening...\n\n");
    }

    int cnt = 0;
    while (1) {
        // Accept clients and store their information in cliAddr
        addr_size = sizeof(cliAddr);
        clientSocket = accept(sockfd, (struct sockaddr *)&cliAddr, &addr_size);

        // Error handling
        if (clientSocket < 0) {
            exit(1);
        }

        // Display information of connected client
        printf("Connection accepted from %s:%d\n", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));

        // Print the number of clients connected till now
        printf("Clients connected: %d\n\n", ++cnt);

        // Create a thread to handle the client
        int *clientSocketPtr = (int *)malloc(sizeof(int));
        *clientSocketPtr = clientSocket;
        pthread_t tid;
        pthread_create(&tid, NULL, clientHandler, (void *)clientSocketPtr);
        pthread_detach(tid); // Detach the thread to clean up resources
    }

    // Close the server socket id (not reached in this code)
    close(sockfd);

    return 0;
}
