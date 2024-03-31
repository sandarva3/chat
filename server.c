#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<string.h>
#include<ws2tcpip.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define MAX_CLIENTS 4
#define INET_ADDRSTRLEN 16
//THIS IS FOR WINDOWS OS. I DIDN'T MODIFIED SERVER FILE TO MAKE IT COMPATILBE ON UNIX-LIKE OS.
//IN LINE with 'UPDATED': we will be printing client's name not ipAddress, so we remove those lines.

int main(int argc, char const* argv[]) {
    WSADATA wsaData;
    SOCKET mastersockfd, connfds[MAX_CLIENTS];
    int activeconnections = 0;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Error Initializing the socket connection(winsock)");
        exit(1);
    }

    if ((mastersockfd = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0)) == INVALID_SOCKET) {
        perror("Error creating a socket.");
        WSACleanup();
        exit(1);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8081);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    if (setsockopt(mastersockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(opt)) == SOCKET_ERROR) {
        perror("Error setting SO_REUSEADDR");
        closesocket(mastersockfd);
        WSACleanup(); // Clean up Winsock
        exit(1);
    }

    // Bind the socket
    if (bind(mastersockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        perror("Bind failed");
        closesocket(mastersockfd);
        WSACleanup();
        exit(1);
    }

    if (listen(mastersockfd, SOMAXCONN) == SOCKET_ERROR) {
        perror("Listen failed");
        closesocket(mastersockfd);
        WSACleanup();
        exit(1);
    }


    // Initialize fd_set for I/O multiplexing
    fd_set readfds;
    int max_fd;
    int readyfds;

    while (1) {
        // Clear readfds set
        FD_ZERO(&readfds);

        FD_SET(mastersockfd, &readfds);
        max_fd = mastersockfd;

        // Add active client sockets
        for (int i = 0; i < activeconnections; i++) {
            if (connfds[i] != INVALID_SOCKET) {
                FD_SET(connfds[i], &readfds);
                if (connfds[i] > max_fd)
                    max_fd = connfds[i];
            }
        }

        // I/O multiplexing
        readyfds = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (readyfds == SOCKET_ERROR) {
            perror("Select failed");
            break;
        }

        // Accept new connections
        if (FD_ISSET(mastersockfd, &readfds)) {
            struct sockaddr_in client_addr;
            int addrlen = sizeof(client_addr);
            if ((connfds[activeconnections] = accept(mastersockfd, (struct sockaddr*)&client_addr, &addrlen)) == INVALID_SOCKET) {
                perror("Accept error");
                break;
            }

            printf("New connection from %s\n", inet_ntoa(client_addr.sin_addr));
            activeconnections++;
        }

        // Read data from clients
        // UPDATED, We won't use this now: char inBuffer[MAX_CLIENTS][1024], outBuffer[MAX_CLIENTS][1024];
        char inBuffer[MAX_CLIENTS][1024];
        for (int i = 0; i < activeconnections; i++) {
            // Check if connection is active and it is ready to read
            if (connfds[i] != INVALID_SOCKET && FD_ISSET(connfds[i], &readfds)) {
                // Clear buffer
                memset(inBuffer[i], 0, 1024);
                // UPDATED, WE NOW REMOVE THIS LINE: memset(outBuffer[i], 0, 1024);

                // recv() returns 0 if connection closed normally
                // and -1 if error
                if (recv(connfds[i], inBuffer[i], 1024, 0) <= 0) {
                    printf("Host disconnected\n");
                    closesocket(connfds[i]); // Close the socket
                    connfds[i] = INVALID_SOCKET;
                    continue;
                }

                // UPDATED, Get client IP. This is that part of code that was used to get client IP. We don't need to get it now, we can simply 
                //print the messages that we get from client. Everything will be handeled smoothly.
                // struct sockaddr_in client_addr;
                // int addrlen = sizeof(client_addr);
                // getpeername(connfds[i], (struct sockaddr*)&client_addr, &addrlen);
                // strncpy(outBuffer[i], inet_ntoa(client_addr.sin_addr), INET_ADDRSTRLEN);

                // printf("%s: %s", outBuffer[i], inBuffer[i]);

                // strcat(outBuffer[i], " : ");
                // strcat(outBuffer[i], inBuffer[i]);

                printf("%s", inBuffer[i]);

                // Write message to each client
                for (int j = 0; j < activeconnections; j++) {
                    if (connfds[j] != INVALID_SOCKET && i != j) {
                        // UPDATED. We don't use this now: send(connfds[j], outBuffer[i], strlen(outBuffer[i]), 0);
                        send(connfds[j], inBuffer[i], strlen(inBuffer[i]), 0);
                    }
                }
            }
        }

    }


    WSACleanup();
    return 0;
}