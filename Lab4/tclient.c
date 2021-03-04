// Jack Davey
// 4-29-2020
// Lab 4 TCP Client
//This file handles the TCP Client for client-server file distribution

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int main(){
    int sockfd, nr;
    char sbuf[1024], rbuf[1024], filename[50];
    
    //Declare server address to connect to
    struct sockaddr_in servAddr;
    struct hostent *host;
    
    printf("Please enter source filename: \n");
    scanf("%s", filename);
    FILE *fp = fopen(filename, "r");
    if(!fp){
        printf("Could not open source file\n");
        return 0;
    }

    host = (struct hostent *)gethostbyname("localhost");
        
     if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
     }

    //Set the server address to send using socket addressing structure
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(5000);
    servAddr.sin_addr = *((struct in_addr *)host->h_addr);

    //Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))){
        perror("Failure to connect to the server");
        exit(1);
    }
    
    while(!feof(filename)){
        int character = fread(sbuf, 1, sizeof(sbuf), filename);
        sendto(sockfd, sbuf, character, 0, (struct sockaddr *)&servAddr, sizeof(struct sockaddr));
    }
     //Close socket descriptor
     close(sockfd);
     return 0;
}
