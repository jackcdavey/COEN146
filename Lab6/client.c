// Jack Davey
// 5/18/20
// Lab 6 Client
// This program serves as the RDT 3.0 client implementation

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "lab6.h"
#define PM 10
#include <netdb.h>

void ClientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet, unsigned retries) {

    while (1) {
        // calculate the checksum
		packet.header.cksum = getcheckSum(packet);
        
        // log what is being sent
        printf("Created: ");
        logPacket(packet);

        // Simulate lost packet.
        if (rand() % PM == 0) {
            printf("Dropping packet\n");
        } else {
            if (sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *) address, addrlen) < 0){
            	printf("Sending Packet");
            	exit(1);
            }
            printf("Awaiting ACK");
        }
        // wait until either a packet is received or timeout, i.e using the select statement
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        fd_set readfds;

		fcntl(sockfd, F_SETFL, O_NONBLOCK);
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        if(select(sockfd + 1, &readfds, NULL, NULL, &tv) == 0){
        	printf("Timeout \n");
        	--retries;
        	//continue;
        }
        // receive an ACK from the server
        Packet recvpacket;
        recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0, address, &addrlen);

        // log what was received
        int sentSeq = packet.header.seq_ack;
        int rcvdAck = recvpacket.header.seq_ack;
        int rcvdExp = recvpacket.header.cksum;
        int rcvdAct = getChecksum(recvpacket);
        printf("Received ACK %d, checksum %d - ", recvpacket.header.seq_ack, recvpacket.header.cksum);

        // validate the ACK
        if (rcvdExp != rcvdAct) {
            // bad checksum, resend packet
            printf("Bad checksum, expected %d\n", getChecksum(recvpacket));
        } else if (rcvdAck != sentSeq) {
            // incorrect sequence number, resend packet
            printf("Bad seqnum, expected %d\n", packet.header.seq_ack);
        } else {
            // good ACK, we're done
            printf("Good ACK\n");
            break;
        }
    }
}

void gPack(Packet pkt) {
	printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"", pkt.header.seq_ack, pkt.header.len, pkt.header.cksum);
	fwrite(pkt.data, (size_t)pkt.header.len, 1, stdout); 
	printf("\"} \n");
}

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

int main(int argc, char *argv[]) {
    Packet endPkt;
	struct sockaddr_in servaddr;
	struct sockaddr_in addr;
    struct hostent *host;
    int sockfd;
    int seq = 0;

    // check arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip> <port> <infile>\n", argv[0]);
        return 1;
    }

    // seed the RNG
    srand((unsigned)time(NULL));

    // create and configure the socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM,0)) < 0){
    	perror("Couldn't setup endpoint socket");
    	exit(1);
    }
	memset(&addr, 0, sizeof(addr));
    host = (struct hostent *)gethostbyname(argv[1]);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr = *((struct in_addr *)host->h_addr);

    // open file
    FILE *file = fopen(argv[3], "rb");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    // send file contents to server
    Packet packet;
    int r=fread(&packet.data, 1, sizeof(packet.data), file);
    while(r >0){
    	Packet pkt;
    	pkt.header.seq_ack = seq;
    	pkt.header.len = r;
    	pkt.header.cksum = getChecksum(pkt);
    	ClientSend(sockfd, (struct sockaddr *)&addr, sizeof(addr), pkt, -1);
    	seq = seq ^ 1;
    	r = fread(&pkt.data, 1, sizeof(pkt.data), file);
    }
    // send zero-length packet to server to end connection according the description provided in the Lab manual
	endPkt.header.seq_ack = seq;
    endPkt.header.len = 0;
    endPkt.header.cksum = getChecksum(endPkt);
    printf("Sending empty final packet");
    logPacket(endPkt);
    int retries = 3;
    ClientSend(sockfd, (struct sockaddr *)&addr, sizeof(addr), endPkt, retries);
    // clean up
	fclose(file);
    close(sockfd);
    return 0;
}
