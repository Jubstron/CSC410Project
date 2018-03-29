#include <stdio.h>
#include <stdlib.h>
 
#define limit 600
 
int main(){
    unsigned long long int i, j;
    int *prime;
    int z = 1;
 
    prime = malloc(sizeof(int) * limit);
 
    for (i = 2; i < limit; i++)
        prime[i] = 1;
 
    for (i = 2; i < limit; i++)
        if (prime[i])
            for (j = i; i * j < limit; j++)
                prime[i * j] = 0;
 
    printf("First 100 prime numbers are: \n");
    int counter = 0;
    for (i = 2; counter < 100; i++)
        if (prime[i]){
	    counter++;
            printf("%d\t", i);
	    if(counter%10==0)
		printf("\n");
	}
 
    return 0;
}
