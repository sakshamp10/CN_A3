// // Server side program that sends
// // a 'hi client' message
// // to every client concurrently

// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <unistd.h>

// // PORT number
// #define PORT 4444

// int main()
// {
// 	// Server socket id
// 	int sockfd, ret;

// 	// Server socket address structures
// 	struct sockaddr_in serverAddr;

// 	// Client socket id
// 	int clientSocket;

// 	// Client socket address structures
// 	struct sockaddr_in cliAddr;

// 	// Stores byte size of server socket address
// 	socklen_t addr_size;

// 	// Child process id
// 	pid_t childpid;

// 	// Creates a TCP socket id from IPV4 family
// 	sockfd = socket(AF_INET, SOCK_STREAM, 0);

// 	// Error handling if socket id is not valid
// 	if (sockfd < 0) {
// 		printf("Error in connection.\n");
// 		exit(1);
// 	}

// 	printf("Server Socket is created.\n");

// 	// Initializing address structure with NULL
// 	memset(&serverAddr, '\0',
// 		sizeof(serverAddr));

// 	// Assign port number and IP address
// 	// to the socket created
// 	serverAddr.sin_family = AF_INET;
// 	serverAddr.sin_port = htons(PORT);

// 	// 127.0.0.1 is a loopback address
// 	serverAddr.sin_addr.s_addr
// 		= inet_addr("10.0.2.15");

// 	// Binding the socket id with
// 	// the socket structure
// 	ret = bind(sockfd,
// 			(struct sockaddr*)&serverAddr,
// 			sizeof(serverAddr));

// 	// Error handling
// 	if (ret < 0) {
// 		printf("Error in binding.\n");
// 		exit(1);
// 	}

// 	// Listening for connections (upto 10)
// 	if (listen(sockfd, 10) == 0) {
// 		printf("Listening...\n\n");
// 	}

// 	int cnt = 0;
// 	while (1) {

// 		// Accept clients and
// 		// store their information in cliAddr
// 		clientSocket = accept(
// 			sockfd, (struct sockaddr*)&cliAddr,
// 			&addr_size);

// 		// Error handling
// 		if (clientSocket < 0) {
// 			exit(1);
// 		}

// 		// Displaying information of
// 		// connected client
// 		printf("Connection accepted from %s:%d\n",
// 			inet_ntoa(cliAddr.sin_addr),
// 			ntohs(cliAddr.sin_port));

// 		// Print number of clients
// 		// connected till now
// 		printf("Clients connected: %d\n\n",
// 			++cnt);

// 		// Creates a child process
// 		if ((childpid = fork()) == 0) {

// 			// Closing the server socket id
// 			close(sockfd);

// 			// Send a confirmation message
// 			// to the client
// 			send(clientSocket, "hi client",
// 				strlen("hi client"), 0);
// 		}
// 	}

// 	// Close the client socket id
// 	close(clientSocket);
// 	return 0;
// }

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

unsigned int min(unsigned int a, unsigned int b){
    if(a < b) return a;
    return b;
}

unsigned int fact(unsigned int n){
    unsigned int i = 1;
    for(unsigned int j = 2; j <= min(n, 20); j++){
        i = i * j;
    }
    return i;
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
    socklen_t addr_size = sizeof(cliAddr);

    // Child process id
    pid_t childpid;

    // Creates a TCP socket id from IPV4 family
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Error handling if socket id is not valid
    if (sockfd < 0) {
        printf("Error in connection.\n");
        exit(1);
    }
    printf("Server Socket is created.\n");

    // Initializing address structure with NULL
    memset(&serverAddr, '\0', sizeof(serverAddr));

    // Assign port number and IP address to the socket created
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Binding the socket id with the socket structure
    ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    // Error handling
    if (ret < 0) {
        printf("Error in binding.\n");
        exit(1);
    }

    // Listening for connections (up to 10)
    if (listen(sockfd, 10) == 0) {
        printf("Listening...\n\n");
    } else {
        printf("Error in listening.\n");
        exit(1);
    }

    int cnt = 0;
    while (1) {
        // Accept clients and store their information in cliAddr
        clientSocket = accept(sockfd, (struct sockaddr*)&cliAddr, &addr_size);

        // Error handling
        if (clientSocket < 0) {
            perror("accept failed");
            continue;
        }

        // Displaying information of connected client
        printf("Connection accepted from %s:%d\n", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));

        // Print the number of clients connected till now
        printf("Clients connected: %d\n\n", ++cnt);

        // Creates a child process
        if ((childpid = fork()) == 0) {
            close(sockfd); // Child doesn't need the listener

            // Handle the client.
            char buffer[1024];
            int bytesRead;

            // Send a confirmation message to the client
            send(clientSocket, "hi client", strlen("hi client"), 0);

            // while (1) {
            //     bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            //     if (bytesRead <= 0) {
            //         break; // Exit the loop when the client disconnects
            //     }

            //     buffer[bytesRead] = '\0'; // Null-terminate the received data
            //     unsigned int result = atoi(buffer);
            //     unsigned int ans = fact(result);

            //     // Compute the length of the answer
            //     char charArray[1024]; // Ensure the buffer is large enough
            //     snprintf(charArray, sizeof(charArray), "%u", ans);

            //     // Send the computed factorial back to the client
            //     send(clientSocket, charArray, strlen(charArray), 0);
            // }
            while (1) {
    bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        break; // Exit the loop when the client disconnects
    }

    buffer[bytesRead] = '\0'; // Null-terminate the received data
    unsigned int result = atoi(buffer);
    unsigned int ans = fact(result);

    // Compute the length of the answer
    char charArray[1024]; // Ensure the buffer is large enough
    snprintf(charArray, sizeof(charArray), "%u\n", ans); // Add a newline character

    // Send the computed factorial back to the client
    send(clientSocket, charArray, strlen(charArray), 0);
            }

            close(clientSocket);
            exit(0); // End the child process
        }
        // The parent process continues to accept new connections
        close(clientSocket); // Parent doesn't need this
    }

    // The following code is theoretically unreachable
    close(sockfd);
    return 0;
}
