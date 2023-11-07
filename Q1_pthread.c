#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h> // Include the pthread library

#define PORT 4444

int clientSocket;

void *receiveMessage(void *arg) {
    char buffer[1024];
    while (1) {
        if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0) {
            printf("Error in receiving data.\n");
        } else {
            printf("Server: %s\n", buffer);
            memset(buffer, 0, sizeof(buffer));
        }
    }
    return NULL;
}

int main() {
    int ret;
    struct sockaddr_in cliAddr;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        printf("Error in connection.\n");
        exit(1);
    }
    printf("Client Socket is created.\n");

    memset(&cliAddr, '\0', sizeof(cliAddr));
    memset(buffer, '\0', sizeof(buffer));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.2.15");

    ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    if (ret < 0) {
        printf("Error in connection.\n");
        exit(1);
    }

    printf("Connected to Server.\n");

    pthread_t receiveThread;
    // Create a thread for receiving messages
    if (pthread_create(&receiveThread, NULL, receiveMessage, NULL) != 0) {
        perror("Failed to create receive thread");
        exit(1);
    }

    while (1) {
        // Handle user input
        char userMessage[1024];
        printf("You: ");
        fgets(userMessage, sizeof(userMessage), stdin);
        userMessage[strcspn(userMessage, "\n")] = '\0'; // Remove the newline character

        // Send user's message to the server
        send(clientSocket, userMessage, strlen(userMessage), 0);

        // Check if the user wants to exit
        if (strcmp(userMessage, "exit") == 0) {
            printf("Exiting the chat.\n");
            break; // Exit the loop and close the program
        }
    }

    pthread_join(receiveThread, NULL); // Wait for the receive thread to finish
    close(clientSocket); // Close the socket when done

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
#include <pthread.h> // Include the pthread library

#define PORT 4444

int clientSocket;

void *receiveMessage(void *arg) {
    char buffer[1024];
    while (1) {
        if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0) {
            printf("Error in receiving data.\n");
        } else {
            printf("Server: %s\n", buffer);
            memset(buffer, 0, sizeof(buffer));
        }
    }
    return NULL;
}

int main() {
    int ret;
    struct sockaddr_in cliAddr;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        printf("Error in connection.\n");
        exit(1);
    }
    printf("Client Socket is created.\n");

    memset(&cliAddr, '\0', sizeof(cliAddr));
    memset(buffer, '\0', sizeof(buffer));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.2.15");

    ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    if (ret < 0) {
        printf("Error in connection.\n");
        exit(1);
    }

    printf("Connected to Server.\n");

    pthread_t receiveThread;
    // Create a thread for receiving messages
    if (pthread_create(&receiveThread, NULL, receiveMessage, NULL) != 0) {
        perror("Failed to create receive thread");
        exit(1);
    }

    while (1) {
        // Handle user input
        char userMessage[1024];
        printf("You: ");
        fgets(userMessage, sizeof(userMessage), stdin);
        userMessage[strcspn(userMessage, "\n")] = '\0'; // Remove the newline character

        // Send user's message to the server
        send(clientSocket, userMessage, strlen(userMessage), 0);

        // Check if the user wants to exit
        if (strcmp(userMessage, "exit") == 0) {
            printf("Exiting the chat.\n");
            break; // Exit the loop and close the program
        }
    }

    pthread_join(receiveThread, NULL); // Wait for the receive thread to finish
    close(clientSocket); // Close the socket when done

    return 0;
}
