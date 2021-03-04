//Jack Davey
//4/24/20
//Lab 3 Part 2
//This program runs a UDP server for transmitting a file

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(){
    int sockfd;
    char filename[50];

    //Declare receiving buffer of size 1k bytes
    char rbuf[1024];
    
    //Declare server address to which to bind for receiving messages and client address to fill in sending address
    struct sockaddr_in servAddr, clienAddr;
    socklen_t addrLen = sizeof(struct sockaddr);
    
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }
    
    
    
    //Setup the server address to bind using socket addressing structure
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(5000); //Port 5000 is assigned
    servAddr.sin_addr.s_addr = INADDR_ANY; //Local IP address of any interface is assigned (generally one interface IP address)
    
    //Set address/port of server endpoint for socket socket descriptor
    if ((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) < 0){
        perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }
    
    printf("Please enter destination filename: \n");
    scanf("%s", filename);
    FILE *fp = fopen(filename, "w");

    
    //Sever continuously waits for messages from client, then prints incoming messages.
    while (1){
        printf("Server waiting for messages from client: \n");
        char data[1024];
        int nr = recvfrom(sockfd, data, 1024, 0, (struct sockaddr *)&clienAddr, &addrLen);
        data[nr] = '\0';
        printf("Client with IP: %s and Port: %d sent message: %s\n", inet_ntoa(clienAddr.sin_addr),ntohs(clienAddr.sin_port), data);
        fwrite(data, 1, sizeof(data), filename);
    }
    
    return 0;
}
