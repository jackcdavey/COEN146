//Jack Davey
// 5/18/20
// Lab 6 Server
// This program serves as the RDT 3.0 server implementation

//Explain what each library does in the context of this lab. Do we need all of them?
// 1 provides internet operation defintions; 2 provides definitions for basic C input/output;
// 3 provides basic C programming definitions; 4 allows usage of strings in place of char arrays
// 5 provides socket address & IP definitions; 6 allows for interaction with system clock, used in RNG
// 7 provides POSIX definitions & API access; 8 provides definitions for unused functions  
#include <arpa/inet.h> // 1 Need
#include <stdio.h> // 2 Need
#include <stdlib.h> // 3 Need
#include <string.h> // 4 Not (?)
#include <sys/socket.h> // 5 Need
#include <time.h> // 6 Need
#include <unistd.h> // 7 Need
#include "lab6.h" // 8 Not
#include <netdb.h>

#define PM 10

//add the missing functions
int gCheck(Packet pkt){
    pkt.header.cksum = 0;
    int checksum = 0; 
    char *ptr = (char *)&pkt;
    char *end = ptr + sizeof(Header) + pkt.header.len;
    while(ptr < end) {
        checksum ^= *ptr++;
    }
    return checksum;
}

void gPack(Packet pkt) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"", pkt.header.seq_ack, pkt.header.len, pkt.header.cksum);
    fwrite(pkt.data, (size_t)pkt.header.len, 1, stdout); 
    printf("\"} \n");
}

void ServerSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) {
    // Simulatting a chance that ACK gets lost
    if (rand() % PM == 0) {
        return;
    }
    //prepare and send the ACK
    Packet packet;
    packet.header.seq_ack = seqnum;
    packet.header.len = sizeof(packet.data);
    sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
    printf("Sent ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);
}

Packet ServerReceive(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum){              
    Packet packet;
    while (1) {
            recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);
            if (packet.header.len == 0) {
                printf("Received empty packet");
                printf("Sending ACK...");
                ServerSend(sockfd, (struct sockaddr *)&address, sizeof(address), seqnum);
                break;
            }
        // log what was received
        printf("Received: ");
        logPacket(packet);

        //verify the checksum and the sequence number
        if (packet.header.cksum != getChecksum(packet)) {
            printf("Bad checksum, expected %d\n", getChecksum(packet));
            ServerSend(sockfd, (struct sockaddr *)&address, sizeof(address), !seqnum);
        } else if (packet.header.seq_ack != seqnum) {
            printf("Bad seqnum, expected %d\n", seqnum);
            ServerSend(sockfd, (struct sockaddr *)&address, sizeof(address),!seqnum);
        } else {
            printf("Good packet\n");
            ServerSend(sockfd, (struct sockaddr *)&address, sizeof(address), seqnum);
        }
    }

    printf("\n");
    return packet;
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in addr;
    struct sockaddr_in clientaddr;
    socklen_t clientaddr_length = sizeof(clientaddr);

    // check arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <outfile>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // seed the RNG
    srand((unsigned)time(NULL));

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Could not setup endpoint socket.");
        exit(1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = INADDR_ANY;
    if ((bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr))) <0 ){
        perror("Couldn't bind server address to endpoint socket.");
        exit(1);
    }

    //Please answer the following questions
        //[Q1] Briefly explain what is "AF_INET" used for.
        //
        //AF_INET is a distiction for sockets your socket can communicate with
        //
        //[Q2] Briefly explain what is "SOCK_DGRAM" used for.
        //
        //Pings a socket by sending and recieving a datagram and closing
        //
        //[Q3] Briefly explain what is "htons" used for.
        //
        //Converts a short from host byte order to network byte order
        //
        //[Q4] Briefly explain why bind is required on the server and not on the client.
        //
        //The client picks the port the server must bind to
        //
    
    
    // open file
    FILE *file = fopen(argv[2], "wb");
    if (file == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    // get file contents from client and save it to the file
    int seqnum = 0;
    Packet packet;
    do {
        packet = ServerReceive(sockfd, (struct sockaddr*)&clientaddr, &clientaddr_length, seqnum);
        write(file, packet.data, srtlen(packet.data));
        seqnum = (seqnum + 1 ) % 2;
    } while (packet.header.len != 0);
    fclose(file);
    close(sockfd);    
    return 0;
}
