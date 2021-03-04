//Jack Davey
//4/29/20
//Lab 4
//This program serves as a client in UDP P2P file transfer

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void client();
void server();
void openSocket();

int sockfd;
struct sockaddr_in servAddr, remoteAddr;
struct hostent *host;
socklen_t addrLen = sizeof(struct sockaddr);
char host[50], source[50], destination[50], rbuf[1024], sbuf[1024];

void openSocket(){
    host = (struct hostent *)gethostbyname(host);
    if (host == NULL) {
        printf("\n Could not resolve hostname \n");
        return;
    }
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(5000); //Port 5000 is assigned
    servAddr.sin_addr = *((struct in_addr *)host->h_addr);
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(6000); //Port 6000 is assigned
    remoteAddr.sin_addr = *((struct in_addr *)host->h_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Could not setup endpoint socket");
        return;
    }
}

void server(){
    int sz;
    gethostname(host, 50);
    openSocket();
    if ((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) < 0){
        printf("\n Failure to bind server address \n");
        return;
    }

    while ((sz = recvfrom(sockfd, rbuf, 1024, 0, (struct sockaddr *)&remoteAddr, &addrLen)) > 0){
        rbuf[sz] = '\0';
        if (strcmp(rbuf, "REQUEST") == 0) {
            break;
        }
    }

    int fp_re = open(source, O_RDWR);
    if (fp_re < 0) {
        perror("Error opening source file!");
        sendto(sockfd, "ERROR", strlen("ERROR"), 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));
        exit(1);
    }
    sendto(sockfd, "SENDING", strlen("SENDING"), 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));
    size_t bytes;
    int file_bytes = 0;
    while (0 < (bytes = read(fp_re, sbuf, sizeof(sbuf)))) {
        sendto(sockfd, sbuf, bytes, 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));
    }

    sendto(sockfd, "EOF", sizeof("EOF"), 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));
    close(fp_re);
    puts("Successfully sent file to client \n");
    close(sockfd);
}

int main() {
    while (1) {
        int input;
        printf("\nEnter: \n 1 : Recieve\n 2 : Send\n 3 : quit\n");
        scanf("%d", &input);
        if (input == 0) {
            printf("Please input filename.\n");
            scanf("%s", source);
            server();
        }
        else if (input == 1) {
            printf("Please ipnut file destination. \n");
            scanf("%s", destination);
            client();
        }
        else if (input == 2) {
            return 0;
        }
        else {
            printf("Invalid input\n");
        }
    }
    return 0;
}
