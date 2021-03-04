// Name: Jack Davey
// Date: 4/13/20
// Lab 2 Pt 1
// Description: This program uses multithreading for matrix manipulation
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define N 2000
#define M 2000
#define L 2000

pthread_t threads[N];
double matrixA[N][M], matrixB[M][L], matrixC[N][L];

void *compute(void *arg);

int main(){
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            matrixA[i][j] = rand(); // rand()
        }
    }
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < L; j++) {
            matrixB[i][j] = rand(); // rand()
        }
    }
    for (int i = 0; i < N; i++){
        int *arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&threads[i], NULL, compute, arg);
    }
    printf("Matrix A\n");
    for (int i = 0; i < N; i++) {
        printf("\n");
        for (int j = 0; j < M; j ++) {
            printf("%f  ", matrixA[i][j]);
        }
    }
    printf("Matrix B\n");
    for (int i = 0; i < M; i++) {
        printf("\n");
        for (int j = 0; j < L; j ++) {
            printf("%f  ", matrixB[i][j]);
        }
    }
    printf("Matrix C\n");
    for (int i = 0; i < N; i++) {
        printf("\n");
        for (int j = 0; j < L; j ++) {
            printf("%f  ", matrixC[i][j]);
        }
    }
  return 0;
}

void *compute(void *arg){
    int i = *(int *)arg;
    for(int j = 0; j < L; j++)
        for (int k = 0; k < M; k++)
            matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
    free(arg);
    return 0;
}
