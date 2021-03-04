// Name: Jack Davey
// Date: 4/4/20
// Lab 1 Step 5
// Description: This program is a recreation of Step 1 using threading rather than forking.
#include <pthread.h>
#include <stdio.h>      /* printf, stderr */
#include <sys/types.h>  /* pid_t */
#include <unistd.h>     /* fork */
#include <stdlib.h>     /* atoi */
#include <errno.h>      /* errno */

void *start(void *arg);

int main(int argc, char *argv[]){
    pid_t pid;
    int i, n = atoi(argv[1]);
    pthread_t tid;
    pthread_create(&tid, NULL, start, &n);
    for (i=0;i<100;i++) {
        printf("\t \t \t Parent Process %d \n",i);
        usleep(n);
    }
    pthread_join(tid, NULL);
    return 0;
}

void *start(void *arg){
    for(int i=0; i<100; i++){
        printf("Child process %d\n",i);
        usleep(*(int*)arg);
    }
    return 0;
}
