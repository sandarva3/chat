#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
//SO THIS IS SAME AS 'Wclient.c' file BUT I MODIFIED IT TO RUN IT ON MY MOBILE PHONE.
//SO This is to be run on android or linux or other unix-like OS.

#define MAX_BUFFER_SIZE 1024

// Global socket descriptor
int sockfd;
char username[100];

//reading from stdin and writing to the socket
void* send_thread(void* data) {
    char sendline[MAX_BUFFER_SIZE];
    while (fgets(sendline, MAX_BUFFER_SIZE, stdin) != NULL) {
        char message[MAX_BUFFER_SIZE + 100];
        sprintf(message, "%s: %s", username, sendline);
        send(sockfd, message, strlen(message), 0);
    }
    return NULL;
}

int main() {
    struct sockaddr_in serv_addr;
    char recvline[MAX_BUFFER_SIZE];

    printf("Please Enter your name: ");
    fgets(username, 100, stdin);
    username[strcspn(username, "\n")] = 0;

    // Create a socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creating socket");
        return -1;
    }

    // Set up the server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8081);
    if (inet_pton(AF_INET, "192.168.1.109", &serv_addr.sin_addr) <= 0) {
        perror("Error setting up server address");
        return -1;
    }

    // Establish a connection with the server
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to the server");
        return -1;
    }

    // Create a new thread for sending data
    pthread_t tid;
    pthread_create(&tid, NULL, send_thread, NULL);

    while (1) {
        memset(recvline, 0, MAX_BUFFER_SIZE);

        // Read from the server
        if (read(sockfd, recvline, MAX_BUFFER_SIZE) > 0) {
            printf("%s", recvline);
        }
    }

    return 0;
}
