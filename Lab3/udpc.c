//Jack Davey
//4/24/20
//Lab 3 Part 2
//This program runs a UDP client for transmitting a file

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

int main(){
    int sockfd;
    char filename[50];
    
    printf("Please enter source filename: \n");
    scanf("%s", filename);
    FILE *fp = fopen(filename, "r");
    if(!fp){
        printf("Could not open source file\n");
        return 0;
    }
    
    //Declare sending buffer of size 1k bytes
    char sbuf[1024];
    
    //Declare server address
    struct sockaddr_in servAddr;
    
    struct hostent *host;
    host = (struct hostent *)gethostbyname("localhost"); //Local host runs the server. You may use "127.0.0.1": loopback IP address
    
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }
    
    //Set the server address to send using socket addressing structure
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(5000);
    servAddr.sin_addr = *((struct in_addr *)host->h_addr);
    
    while (!feof(fp)) {
        int c = fread(sbuf, 1, sizeof(sbuf), fp);
        sendto(sockfd, sbuf, c, 0, (struct sockaddr *)&servAddr, sizeof(struct sockaddr));
    }
    
    return 0;
}
