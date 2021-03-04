//Lab 7
//Jack Davey
//6/1/20
//This lab uses the link-state algorithm to send and decieve data on a router

#include <sys/select.h>
#include <sys/time.h>
#include <limits.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>

//defines
#define N           4
#define INFINITE    1000

typedef struct {
        char name[50];
        char ip[50];
        int port;
} MACHINES;

int i;
int j;
int in[3];
int out[3];
int host1;
int host2;
int weight;
int costs[100][100];
int k;
int l; 
int min;
int m;
int mindex;
int wait_time;
int a;
int b;
int src;
int ct;
int d;
int sock;
int x;
int nhbr;
int new_cost;
int y;
int z;
int id;
int n;
int port;
int this_sock; 
FILE *fpc, *fph;
pthread_mutex_t mutex;
MACHINES hosts[100];
pthread_t receive;
pthread_t link_st;
time_t updated;

// functions
void *run_link_state (void *);
void *receive_info (void *);
void get_costs (void);
void print_costs (void);

int main (int argc, char *argv[]) {
        if (argc != 5) {
                printf ("Not enough arguments, exiting... \n");
                return 1;
        }
        sscanf (argv[1], "%d", &id);
        sscanf (argv[2], "%d", &n);
        n = 4;
        fpc = fopen(argv[3], "r");
        fph = fopen(argv[4], "r");


        for (i = 0; i < n; i++) {
                for (j = 0; j < n; j++) {
                        if (fscanf (fpc, "%d", &costs[i][j]) != 1)
                                break;
                        printf ("%d ", costs[i][j]);
                }

                printf ("\n");
        }

        for (i = 0; i < n; i++) {
                if (fscanf (fph, "%s %s %d", &hosts[i].name, &hosts[i].ip, &hosts[i].port) < 1)
                        break;
                printf ("%s %s %d\n", hosts[i].name, hosts[i].ip, hosts[i].port);
        }

        port = hosts[id].port;

        struct  sockaddr_in thisAddr, otherAddr;
        struct sockaddr_storage thisStorage;
        socklen_t thisAddr_size, otherAddr_size;

        // configure address
        thisAddr.sin_family = AF_INET;
        thisAddr.sin_port = htons ((short)port);
        thisAddr.sin_addr.s_addr = htonl (INADDR_ANY);
        memset ((char *)thisAddr.sin_zero, '\0', sizeof (thisAddr.sin_zero));
        thisAddr_size = sizeof thisStorage;

        // create socket
        if ((this_sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0) {
                printf ("socket error\n");
                return 1;
        }

        // bind
        if (bind (this_sock, (struct sockaddr *)&thisAddr, sizeof (thisAddr)) != 0)
        {
                printf ("bind error\n");
                return 1;
        }

        pthread_mutex_init(&mutex, NULL);
        pthread_create(&receive, NULL, receive_info, NULL);
        pthread_create(&link_st, NULL, run_link_state, NULL);

        for(int ab=0; ab<2; ab++){
            get_costs();
            sleep(10);
        }
        get_costs();

        return 1;
}


/**********************
 * receive info
 **********************/
//If there is a cost update on other machine, they will let us know through a UDP packet. We need to receive that packet and update our costs table
void *receive_info () {
        while (1) {
                recvfrom (this_sock, &in, sizeof (in), 0, NULL, NULL);
                host1 = ntohl (in[0]);
                host2 = ntohl (in[1]);
                weight = ntohl (in[2]);

                pthread_mutex_lock (&mutex);
                costs[host1][host2] = weight;
                costs[host2][host1] = weight;
                for (k = 0; k < n; k++) {
                        for (l = 0; l < n; l++) {
                                printf ("%d ", costs[k][l]);
                        }

                        printf ("\n");
                }

                printf ("\n");

                pthread_mutex_unlock (&mutex);
        }
}

void *run_link_state (){
        updated = time(NULL);
        while (1) {
                wait_time = (rand()%10 * 2);
                int dist[n], visited[n], temp[n][n];
                if ((time(NULL) - updated) > wait_time) {
                        pthread_mutex_lock (&mutex);
                        for (src = 0; src < n; src++) {
                                for (a = 0; a < n; a++) {
                                        dist[a] = INT_MAX;
                                        visited[a] = 0;
                                }
                                dist[src] = 0;
                                for (ct = 0; ct < n - 1; ct++) {

                                    min = INT_MAX;
                                    for (m = 0; m < n; m++) {
                                        if (visited[m] == 0 && dist[m] < min) {
                                            min = dist[m];
                                            mindex = m;
                                        }
                                    }
                                    d=mindex;
                                    visited[d] = 1;
                                    for (b = 0; b < n; b++) {
                                            if (visited[b] == 0 && dist[d] != INT_MAX && dist[d] + costs[d][b] < dist[b])
                                                    dist[b] = dist[d] + costs[d][b];
                                    }
                                }
                                for (a = 0; a < n; a++) {
                                        temp[src][a] = dist[a];
                                        temp[a][src] = dist[a];
                                }
                        }
                        printf("New costs:\n");
                        for (a = 0; a < n; a++) { 
                                for (b = 0; b < n; b++) {
                                        printf("%d ", temp[a][b]);
                                }

                                printf("\n");
                        }

                        printf("\n");
                }

                pthread_mutex_unlock (&mutex);
                updated = time(NULL);
        }
}

// read changes from the keyboard, update the costs matrix inform other nodes about the changes
void get_costs () { 
    printf ("Enter new cost of node %d in format <cost> <neighbor>\n", id);
    scanf ("%d %d", &new_cost, &nhbr);
    pthread_mutex_lock (&mutex);
    costs[id][nhbr] = new_cost;
    costs[nhbr][id] = new_cost;
    out[0] = htonl (id);
    out[1] = htonl (nhbr);
    out[2] = htonl (new_cost);
    struct sockaddr_in destAddr[n];
    socklen_t addr_size[n];

    for (x = 0; x < n; x++) {
            destAddr[x].sin_family = AF_INET;
            destAddr[x].sin_port = htons (hosts[x].port);
            inet_pton (AF_INET, hosts[x].ip, &destAddr[x].sin_addr.s_addr);
            memset (destAddr[x].sin_zero, '\0', sizeof (destAddr[x].sin_zero));
            addr_size[x] = sizeof destAddr[x];
    }
    sock = socket (PF_INET, SOCK_DGRAM, 0);
    for (x = 0; x < n; x++) {
            if (x != id)
                    sendto (sock, &out, sizeof(out), 0, (struct sockaddr *)&(destAddr[x]), addr_size[x]);
    }
    for (y = 0; y < n; y++) {
            for (z = 0; z < n; z++) {
                    printf ("%d ", costs[y][z]);
            }
            printf ("\n");
    }

    pthread_mutex_unlock (&mutex);
}

/**********************
 * print costs
 **********************/
void print_costs (void){
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            pthread_mutex_lock (&mutex);
            printf ("%d ", costs[i][j]);
            pthread_mutex_unlock (&mutex);
        }
        printf ("\n");
    }

}
