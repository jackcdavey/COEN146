// Jack Davey
// 4-29-2020
// Lab 4 TCP Server
//This file handles the TCP server for client-server file distribution

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define N 5

void *ts(void *arg);
pthread_t tids[N];
FILE *fp;


int main(){
    int sockfd, connfd, rb, sin_size;
    
    //Declare receiving and sending buffers of size 1k bytes
    char rbuf[1024], sbuf[1024];
    
    //Declare server address to which to bind for receiving messages and client address to fill in sending address
    struct sockaddr_in servAddr, clienAddr;
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }
    
    //Setup the server address to bind using socket addressing structure
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(5000);
    servAddr.sin_addr.s_addr = INADDR_ANY;
    
    //Set address/port of server endpoint for socket socket descriptor
    if ((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) < 0){
        perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }
    
    printf("Server waiting for client at port 5000\n");
    
    // Server listening to the socket endpoint, and can queue 5 client requests
    if(listen(sockfd, N)==-1){
        printf("\n Could not connect to client \n");
        return 0;
    }
    for(int i=0; i<=N; i++){
        int *client = malloc(sizeof(int));
        *client = accept(sockfd, (struct sockaddr *)&clienAddr, (socklen_t *)&sin_size);
        pthread_create(&tids, NULL, ts, client);
    }
    fclose(fp);
    close(sockfd);
    return 0;
}
