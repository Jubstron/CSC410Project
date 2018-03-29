/*
* Team #3: Anthony Bisignano, Mario Carl, and Justin Nunez
* Dr. Joe Zhang
* CSC 410
*/

#include <stdio.h>
#include <math.h>
#include <time.h>

int main(){
  int n = 0;
  setbuf(stdout, NULL);

  printf("Enter n: ");
  scanf("%d", &n);
  int arr[n];

  //time_t start = time(NULL);

  // Fill the array with n numbers defaulted to true (1)
  for(int i = 0; i < n; i++){
    arr[i] = 1;
  }

  // Mark all multiplicants as false
  for(int i = 2; i < ceil(sqrt(n)); i++){
    if(arr[i-1]){
      for(int j = 2; j < (n/i)+1; j++){
        arr[(i*j)-1] = 0;
      }
    }
  }
  //time_t end = time(NULL);

  // Print the primes
  //printf("\nTime taken: %d seconds\nPrimes:\n", end-start);
  for(int i = 1; i < n; ++i){
    if(arr[i] == 1){
      printf(" %d\n", i+1);
    }
  }

  return 0;
}
