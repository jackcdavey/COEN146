// Name: Jack Davey
// Date: 4/2/20
// Lab 1 Circuit Switching & Packet Switching
// Description: Compares packet and cicuit switching characteristics qualatatively

#include <stdio.h>
#include <math.h>


long long factorial(int x){
    if(x==0)
        return 1;
    else
        return x*factorial(x-1);
}

int main(){
    int linkBandwidth = 200;
    int userBandwidth = 20;
    int nCSusers = linkBandwidth/userBandwidth;
    int nPSusers = 19;
    double tPSuser = 0.10;
    double pPSusersBusy = tPSuser;
    double pPSusersNotBusy = 1-pPSusersBusy;

    //Step 7a
    double pPSusers = tPSuser;
    
    //Step 7c
    double c7 = pow((1 - pPSusers),(nPSusers - 1));

    //Step 7d
    double d7 = pPSusers * pow(pPSusersNotBusy,(nPSusers-1));

    //Step 7e
    double e7 = nPSusers * d7;

    //Step 7f
    double f7 = pow(pPSusers,10) * pow(pPSusersNotBusy, (nPSusers-10));

    //Step 7g
    int num = factorial(nPSusers);
    int one = factorial(10);
    int two = factorial(nPSusers- 10);
    double factor = num /one;
    factor /= two;
    double g7 = factor * pow(pPSusersBusy,10) * pow(pPSusersNotBusy, (nPSusers - 10));

    //Step 7h
    double h7 = 0;
    for (int j=11;j<nPSusers + 1;++j){
      int num = factorial(nPSusers);
      int a = factorial(j);
      int b = factorial(nPSusers - j);
      double res = num / a;
      res /= b;
      h7 += (res * pow(pPSusersBusy,j) * pow(pPSusersNotBusy,(nPSusers - j)));
    }
    
    printf("nCSusers: %d \n", nCSusers);
    printf("pPSusers: %f \n", pPSusers);
    printf("pPSusersNoyBusy: %f \n", pPSusersNotBusy);
    printf("Prob. that all other users are not busy: %f \n", c7);
    printf("Probability that only one specific user is transmitting: %f \n", d7);
    printf("Probability that any one user is busy: %e \n", e7);
    printf("Probability that 10 specific users are transmitting: %e \n", f7);
    printf("Probability that any 10 users are transmitting: %e \n", g7);
    printf("Probability more than ten users are transmitting: %e \n", h7);

    return 0;
}
