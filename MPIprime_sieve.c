#include <stdio.h>
#include <string.h>  /* For strlen             */
#include <math.h>
#include <mpi.h>     /* For MPI functions, etc */ 

void sieve(char *array, int n);
void print(char *array, int n, int begin);
void sieve_two(char *primes, char *array, int start, int end);
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
		n_end;
	
	
	/* Start up MPI */
	MPI_Init(NULL, NULL); 

  	/* Get the number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 

 	/* Get my rank among all the processes */
  	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
	
	if(my_rank == 0){
		printf("Enter #p & #n \n");
		scanf("%d %d",&p,&n);
		
	}
	
	//Make sure all processors get input values
	MPI_Bcast(&p,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	
	//input protection
	if (p <=0 || p > comm_sz || n <= 0){
		if(my_rank ==0){
			printf("#p must be between 1 and %d\n", comm_sz);
			printf("Or #n must be greater than zero\n");
		}
		MPI_Finalize();
		return 0;
	}
	
	if (my_rank < p){
		// get n starter primes
		n_start_primes = (int)(ceil(sqrt(n)));
		printf("%d\n", n_start_primes);
		
		// allocate starter array
		char *sp_array = malloc(n_start_primes * sizeof(char));
		
		// initialize array to 0
		int i;
		for(i = 0; i < n_start_primes; i++){sp_array[i]=0;}
		
		//compute starter primes
		sieve(sp_array, n_start_primes);
		
		// divide the work. 
		local_n = (n - n_start_primes)/p;		
		extra_n = (n - n_start_primes)%p;	
		int largest_n = local_n + 1;
		if(my_rank < extra_n)
		{
			local_n = local_n+1;
		}
		
		
		n_start = n_start_primes + my_rank*local_n + extra_n*(largest_n - local_n);
		n_end 	= n_start + local_n - 1;
		
		// allocate array
		char *local_array = malloc(local_n * sizeof(char));
		for(i = 0; i < local_n; i++){local_array[i]=0;}
		//compute remaining 
		sieve_two(sp_array, local_array, n_start, n_end);
		
		print(local_array,local_n, n_start);
		free(local_array);
		free(sp_array);
	}
	MPI_Finalize();
	
  
  
   return 0;
}  
void sieve_two(char *primes, char *array, int start, int end){
  int i, k = 2, bias = start/2, adjust = 0;
  if(start%2)
	  adjust=1;
  
  for(i=bias+adjust; i*k <= end; ++i){
      array[i*k-start] = 1;
  }
  k++;
  //k>2

  for(;k*k <= end;){
	  adjust=0;
	  bias=start/k;
	  // if less then start adjust
	  if((start/k)*k < start)
		  adjust++;
	  // if even adjust
	  if(((start/k)+adjust)%2==0)
		  adjust++;
	  
      for(i=(start/k)+adjust; i*k <= end;i=i+2){
        if(array[i*k-start]==0) 
          array[i*k-start] = 1; 
      }
    //skip all even numbers
    k=k+2;
    //skip all marked odd numbers
    for(;primes[k]!=0;k=k+2){}
  }
  
}


void sieve(char *array, int n){
  int i, k = 2;
  for(i=k; i*k < n;++i){
    if(array[i*k]==0)
      array[i*k] = 1;
  }
  k++;
  //k>2

  for(;k*k < n;){
      for(i=k; i*k < n;i=i+2){
        if(array[i*k]==0) 
          array[i*k] = 1; 
      }
    //skip all even numbers
    k=k+2;
    //skip all marked odd numbers
    for(;array[k]!=0;k=k+2){}
  }
  
}
void print(char *array, int n, int begin){
	int i, pcount = 1;
	for( i = 0;pcount<=100 && i<n;++i){
		if (array[i]==0)
		{

		printf("%d:  %d\n",pcount ,i+begin);
		++pcount;
		}	
	}
	/* for( i = 0; i<n;++i){
		printf("%d:  %d     %d\n",pcount ,i, array[i]);
		++pcount;
	} */
}
