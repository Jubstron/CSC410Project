#include <stdio.h>
#include <time.h>
#include <stdlib.h>

inline void myPrint(int *arr, const int num){
  for(int i = 0;i<num;++i){
    if (arr[i]!=0)
    {
      printf("%d\n",arr[i]);
    }
  }
}

int main(){
  clock_t start = clock(), diff;


  //printf("test\n");
  const int n = 10000000; //100 million ~1.3 seconds
  int k = 2;
  //int array[n];
  int *array = malloc(n * sizeof(int));
  int i;
  //k=2
  for(i=k; i*k < n;++i){
    if(array[i*k]==0)
      array[i*k] = i*k;
  }
  k++;
  //k>2
  //saves some time to skip all even numbers
  //maybe patterns can help skip more.
  for(;k*k < n;){
    if(array[k] == 0){
      for(i=k; i*k < n;i=i+2){
        if(array[i*k]==0)   //saves small amount of time
          array[i*k] = 1; // dont need to actually calculate number
      }
    }
    k=k+2;
    for(;array[k]!=0;k=k+2){}
  }
  //myPrint(array, n);
  // for( i = 0;i<n;++i){
  //   if (array[i]==0)
  //   {
  //     printf("%d\n",i);
  //   }
  // }
  diff = clock() - start;

  int msec = diff * 1000 / CLOCKS_PER_SEC;
  printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
  free(array);
  return 0;
}
