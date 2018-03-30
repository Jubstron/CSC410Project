/*
* Team #3: Anthony Bisignano, Mario Carl, and Justin Nunez
* Dr. Joe Zhang
* CSC 410
*/

/*
* Notes:
*   - I plan to switch to a true boolean array
*   - The loops is not currently not checking/ accouting for duplicates
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

int main(){
  int n = 0;
  int count = 0;
  setbuf(stdout, NULL);

  printf("Enter n: ");
  scanf("%d", &n);
  bool *arr = malloc(n*sizeof(bool));

  time_t start = time(NULL);

  // Fill the array with n numbers defaulted to true (1)
  for(int i = 0; i < n; i++){
    arr[i] = 1;
  }

  // Mark all multiplicants as false
  for(int i = 2; i < sqrt(n); i++){
    if(arr[i-1]){
      for(int j = 2; j < (n/i)+1; j++){
        arr[(i*j)-1] = 0;
      }
    }
  }
  time_t end = time(NULL);

  // Print the primes
  printf("\nPrimes:\n");
  for(int i = 1; i < n; ++i){
    if(arr[i] == 1 && count <= 100){
      printf(" %d\t", i+1);
      count++;
    }
  }
  printf("\n\nTime taken: %0.3g seconds\n", difftime(end, start));

  free(arr);
  return 0;
}
