#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main (int argc, char *argv[])
{

        int nthreads;
        int n = 0, i, k;
        nthreads = strtol(argv[1], NULL, 10);
        printf("Enter n: \n");
        scanf("%lld",&n);
        //n= 100;
        char *array = malloc(n * sizeof(char));
        for(i = 0; i < n; i++){array[i]=0;}

        clock_t start = clock(), diff;

        k=2;
        #pragma omp parallel for
        for(i=k; i < n/k;++i){
            array[i*k] = 1;
        }

        k++;
        for(;k*k < n;){

            #pragma omp parallel for
            for(i=k; i <= n/k;i=i+2){
              if(!array[i*k])   //saves small amount of time
                array[i*k] = 1; // dont need to actually calculate number
            }
          //skip all even numbers
          k=k+2;
          //skip all marked numbers
          for(;array[k]!=0;k=k+2){}
        }
        /* Fork a team of threads */
        diff = clock() - start;
        int msec = diff * 1000 / CLOCKS_PER_SEC;

        int pcount = 1;
        for( i = 2;pcount<100 && i<n;++i){
          if (array[i]==0)
          {

            printf("%d:  %d\n",pcount ,i);
            ++pcount;
          }
        }
        printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
}
