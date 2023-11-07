#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// PORT number
#define PORT 4444

int main() {
    // Socket id
    int clientSocket, ret;

    // Server socket structure
    struct sockaddr_in serverAddr;

    // Char array to store incoming message
    char buffer[1024];

    // Creating socket id
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        perror("Error in creating socket");
        exit(1);
    }
    printf("Client Socket is created.\n");

    // Initializing server socket structure with NULL
    memset(&serverAddr, 0, sizeof(serverAddr);

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
        perror("Error in connecting to the server");
        exit(1);
    }

    printf("Connected to Server.\n");

    while (1) {
        // recv() receives the message from the server and stores it in the buffer
        if (recv(clientSocket, buffer, sizeof(buffer) - 1, 0) < 0) {
            perror("Error in receiving data");
            break;
        }

        // Printing the message on the screen
        else {
            printf("Server: %s\n", buffer);
            memset(buffer, 0, sizeof(buffer));
        }
    }

    // Close the client socket
    close(clientSocket);

    return 0;
}
