// Name: Jack Davey
// Date: 4/13/20
// Lab 2 Pt 2
// Description: This program computes a variety of data transfer times.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(){
    int RTT0 = 3;
    int RTT1 = 20;
    int RTT2 = 26;
    int RTTHTTP = 47;
    int obj = 6;
    int secObj = 3;
    
    int a = RTT0 + RTT1 + RTT2 + (2 * RTTHTTP);
    int b = a + (obj * 2 * RTTHTTP);
    int c1 = a + (2 * ceil(obj/secObj) * RTTHTTP);
    int c2 = a + (ceil(obj/secObj) * RTTHTTP);

    printf("Part A: %d ms\n", a);
    printf("Part B: %d msecs\n", b);
    printf("Part C non-persistent: %d ms\n", c1);
    printf("Part C persistent: %d ms\n", c2);
}
