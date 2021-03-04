//Jack Davey
// 4/24/20
//Lab 3 Part 1
//This program can copy a text file to another file
#include <stdio.h>
#include <stdlib.h>

int main(){
    char filename1[50], filename2[50], c;
    printf("Please enter source filename: \n");
    scanf("%s", filename1);
    printf("Please enter destination filename: \n");
    scanf("%s", filename2);
    
    FILE *fp = fopen(filename1, "r");
    FILE *nfp = fopen(filename2, "w");
    
    if(!fp){
        printf("Could not open source file\n");
        return 0;
    }
    if(!nfp){
        printf("Could not open destination file\n");
        return 0;
    }
    
    c = fgetc(fp);
    while (c != EOF){
        fputc(c, nfp);
        c = fgetc(fp);
    }
    
    fclose(fp);
    fclose(nfp);
    return 0;
}
