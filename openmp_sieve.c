/*
* This program discovers prime numbers using the Sieve of Eratosthenes.
* Using a serial implementation, OpenMP is used to parallelize critical loops
* within the code. The program uses one shared array to record all values to be
* marked by the sieve.
*
*/



#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main (int argc, char *argv[])
{

    int nthreads;
    //counters
    long long int i, k, pcount, checked;
    //totals, and n
    long long int n = 0, total_pcount=0, total_checked=0;

    //get input and number of threads from command line
    nthreads = strtol(argv[1], NULL, 10);
    printf("Enter n: \n");
    scanf("%lld",&n);

    //allocate array initiallize to 0
    char *array = malloc(n * sizeof(char));
    for(i = 0; i < n; i++){array[i]=0;}

    //clock variables & start the clock
    double start, diff;
		start = omp_get_wtime( );

    /* ---------------- parallel region ---------------- */
		#pragma omp parallel num_threads(nthreads) \
			private(i,k,checked,pcount) shared(n,array) \
      reduction(+: total_checked, total_pcount)
		{
      //private variables and initiallization
			long long int j=3;
			checked=0;
			k=2;

      //mark all evens in parallel
			#pragma omp for
			for(i=k; i < n/k;++i){
				array[i*k] = 1;
			}

			k++;
			for(;k*k < n;){

        //mark all odd compositions in parallel
				#pragma omp for
				for(i=k; i <= n/k; i+=2 ){

					if(!array[i*k])  		//check if marked before assingment
						array[i*k] = 1;
					else
						checked++;			//if allready marked, reduntant work incremented

				}

			  //skip all even numbers
			  k=k+2;
			  //skip all marked numbers
			  for(;array[k]!=0;k=k+2){}
			}
			total_checked += checked;

		//count the primes in parallel

  		pcount = 0;
  		#pragma omp for
  		for( i = 2;i<n;++i){
  			if (array[i]==0)
  				++pcount;
  		}
  		total_pcount+=pcount;

    }	/* Join a team of threads */

    //stop the clock
    diff = omp_get_wtime()-start;

		//print first 100 primes
		pcount=0;
		for( i = 2; pcount<100 && i<n;++i){
			if (array[i]==0)
			{
				++pcount;
				if(pcount<=100){
					printf("%d:\t%d\t",pcount ,i);

					if(pcount%5==0)
						printf("\n");
				}
			}
		}

		//print results
		printf("\n");
        printf("Total Primes: %lld Total Checked: %lld Time: %lf\n", total_pcount, total_checked,diff);
} //gcc -O2 -fopenmp -o openmp_sieve openMP_sieve_p3.c
