#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<errno.h>
#include<string.h>
#include<process.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define MAX_BUFFER_SIZE 1024
//This is meant to be run on WINDOWS OS.
//For unix-like OS there's file 'Aclient.c'.

// Global socket descriptor
SOCKET sockfd;
char username[100];

//reading from stdin and writing to the socket
void send_thread(void* data) {
    char sendline[MAX_BUFFER_SIZE];
    while (fgets(sendline, MAX_BUFFER_SIZE, stdin) != NULL) {
        char message[MAX_BUFFER_SIZE + 100];
        sprintf(message, "-%s: %s", username, sendline);
        send(sockfd,message,strlen(message), 0);
    }
}

int main() {
    printf("Please Enter your name: ");
    fgets(username, 100, stdin);
    username[strcspn(username, "\n")] = 0;
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to load Winsock");
        return -1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Failed to create socket");
        WSACleanup();
        return -1;
    }

    struct sockaddr_in serv_addr;
    char recvline[MAX_BUFFER_SIZE];

    // Set up the server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8081);
    serv_addr.sin_addr.s_addr = inet_addr("192.168.0.000"); //Enter IP address of server here.

    // Establish a connection with the server
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        perror("Error connecting to the server");
        closesocket(sockfd);
        WSACleanup();
        exit(1);
    }

    // new thread for sending data
    _beginthread(send_thread, 0, NULL);

    while (1) {
        memset(recvline, 0, MAX_BUFFER_SIZE);
        recv(sockfd, recvline, MAX_BUFFER_SIZE, 0);
        fprintf(stdout, "%s", recvline);
    }

    return 0;
}
