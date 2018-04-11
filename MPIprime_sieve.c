#include <stdio.h>
#include <string.h>  /* For strlen             */
#include <math.h>
#include <mpi.h>     /* For MPI functions, etc */

int sieve(char *array, int n);
void print(char *array, int n, int begin);
int sieve_two(char *primes, char *array, int start, int end);
/* main */
int main(void) {
    int my_rank,
    comm_sz,
    p,
    n,
    n_start_primes,
    subset_n,
    local_n,
    extra_n,
    n_start,
    n_end,
    local_check;
    double start_time, stop_time, ticks_start, ticks_stop;
    
    /* Start up MPI */
    MPI_Init(NULL, NULL);
    
    /* Get the number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &p);  // p is now comm_sz
    
    /* Get my rank among all the processes */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    //user input
    if(my_rank == 0){
        printf("Enter #n \n");
        scanf("%ld", &n);
        // no longer set p, MPI_GATHERV uses comm_sz to determin communication.
    }
    
    //Make sure all processors get input values
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    //start timer
    if(my_rank==0){
        //ticks_start = MPI_Wtick();
        start_time = MPI_Wtime();
    }
    
    //input protection
    if ( p > n || n <= 0){
        if(my_rank ==0){
            printf("#n must be greater than zero or >= p: %d \n", p);
        }
        MPI_Finalize();
        return 0;
    }
    
    // loop counters
    int i, j;
    // work arrays
    char *sp_array, *local_array, *final_array;
    
    // for MPI_Gatherv requirments displacments & receiving counts
    int *displs, *rcounts;
    
    // get n starter primes
    n_start_primes = (int)(ceil(sqrt(n)));
    
    // divide the work.
    local_n = (n - n_start_primes)/p;
    
    // have any extras?
    extra_n = (n - n_start_primes)%p;
    
    //keep track of largest possible local_n
    int largest_n = local_n + 1;
    
    //pass extra to to local_n; (extra_n < p) always!
    if(my_rank < extra_n)
    {
        ++local_n;
    }
    // start and end points for each process {(largest_n - local_n) == 1 || 0} always!
    n_start = n_start_primes + my_rank * local_n + extra_n * (largest_n - local_n);
    n_end     = n_start + local_n;
    
    /*----------------------------DO WORK-------------------------------------*/
    if(my_rank == 0){
        //p0 will do work all at once
        //combine starting primes with local_n
        local_n+=n_start_primes;
        
        //allocate and initailize array to 0
        local_array = malloc(local_n * sizeof(char));
        for(i = 0; i < local_n; i++){local_array[i]=0;}
        
        // GO!
        local_check = sieve(local_array, local_n);
        
    } else {
        
        // allocate starter array & initialize array to 0
        sp_array = malloc(n_start_primes * sizeof(char));
        for(i = 0; i < n_start_primes; i++){sp_array[i]=0;}
        
        //compute starter primes
        sieve(sp_array, n_start_primes);
        
        // allocate main array initailize to 0
        local_array = malloc(local_n * sizeof(char));
        for(i = 0; i < local_n; i++){local_array[i]=0;}
        
        //compute primes in local array
        local_check = sieve_two(sp_array, local_array, n_start, n_end);
        
    }
    // put local_n values in an array (needed for MPI_Gatherv)
    rcounts = malloc(p * sizeof(int));
    
    // Each process tells the root how many elements it holds
    MPI_Gather(&local_n, 1, MPI_INT, rcounts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Displacements for MPI_GATHERV, where to place local_array in final array;
    displs = malloc(p * sizeof(int));
    
    // Calculate Displacement; my_rank(0) == 0
    for ( i = 0; i < p; i++)
        displs[i] = (i > 0) ? (displs[i-1] + rcounts[i-1]) : 0;
    
    // Place to hold the gathered data
    final_array = NULL;
    
    // Allocate at root only
    if (my_rank == 0)
        final_array = malloc((displs[p-1]+rcounts[p-1])*sizeof(char));
    
    // Gather using displacments and recv counts
    MPI_Gatherv(local_array, local_n, MPI_CHAR, final_array, rcounts, displs, MPI_CHAR, 0, MPI_COMM_WORLD);
    
    //calculate total primes
    int pcount = 0, total_prime, total_check;
    for(i=(my_rank==0)?2:0; i < local_n; i++)
        if(local_array[i]==0)
            pcount++;
    
    //sum total primes and total checks from each process to root
    MPI_Reduce(&pcount, &total_prime, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_check, &total_check, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    //print results
    if (my_rank == 0){
        //stop time
        stop_time = MPI_Wtime();
        //print first 100 primes
        print(final_array,n,0);
        //print total and time
        printf("Total primes: %d\n", total_prime);
        printf("Total checks: %d\n", total_check);
        printf("Total time: %f seconds\n", stop_time-start_time);
    }
    
    
    MPI_Finalize();
    
    
    
    return 0;
}

/*
 This version of seive handles different starting points
 and uses a secondary array *primes to mark *array
 */
int sieve_two(char *primes, char *array, int start, int end){
    int     i,
    k = 2,
    bias,                    //bias is used to get k*i in range of start
    adjust = 0,                //adjust from even or odd
    checked = 0;            //count nonprime checks
    
    bias = (int)ceil((double)start/(double)2);
    
    // compute all evens first
    for(i=bias; i*k < end; ++i){
        array[i*k-start] = 1;
    }
    k++;
    //k>2
    
    for( ; k*k < end; ){
        adjust=0;
        //get starting point
        bias=(int)ceil((double)start/(double)k);
        if(bias<k)
            bias = k;
        // adjust must be odd.
        if(bias%2 == 0)
            adjust++;
        for(i=(bias)+adjust; i*k < end; i=i+2){
            if(array[i*k-start]==0)
                array[i*k-start] = 1;
            else
                checked++;            // if number is already marked increase checked
        }
        //skip all even numbers
        k = k+2;
        //skip all marked odd numbers
        //use prime array to find the next k.
        for( ;primes[k]!=0; k=k+2){}
    }
    return checked;
}

/*
 serial implementtion of sieve to find starting primes and p0 work
 */
int sieve(char *array, int n){
    int i, k = 2, checked = 0;
    // compute all evens first
    for(i=k; i*k < n;++i){
        array[i*k] = 1;
    }
    k++;
    //k>2
    
    for( ; k*k < n; ){
        for(i=k; i*k < n;i=i+2){
            if(array[i*k]==0)
                array[i*k] = 1;
            else
                checked++;            // if number is already marked increase checked
        }
        //skip all even numbers
        k=k+2;
        //skip all marked odd numbers
        for(;array[k]!=0;k=k+2){}
    }
    return checked;
}

/* print primes */
void print(char *array, int n, int begin){
    int i, pcount = 0;
    for(i = 2; pcount < 100 && i < n; ++i){
        if (array[i]==0)
        {
            ++pcount;
            if (pcount<=100)
                printf("%ld:  %ld\n",pcount ,i+begin);
            
        }
    }
}
