#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/* ugly and slow */

int main(){
  clock_t start = clock(), diff;


  const int n = 100; //10 million
  int bits=32;
  int asize = (n/bits)+1;
  int k = 2;
  int *t = malloc(asize * sizeof(int));
  int i, j, bitcount;
  //k=2

  //unsigned int t[31250];
  //printf("%#010x\n",t[1]-1);
  int mask=1;
  int arrayNum=0;
  int temp = 0;

  for(;k*k < n;){
    //move mask to intital start k*k
    for(j = 1;j<k*k;++j){
      mask <<= 1;
      if(mask==0){
        mask = 1;
        ++arrayNum;
      }
    }
      for(i=k; i*k <= n;++i){
        //mark number
        t[arrayNum] |= mask;
        //move mask k for next iteration.
        for(j = 0;j<k;++j){
          mask <<= 1;
          if(mask==0){
            mask = 1;
            ++arrayNum;
          }
        }

      }
    //increment k and reset
    k++;
    arrayNum = 0;
    mask = 1;
  }

  arrayNum=0;
  mask = 2;
  for( i = 2;i<=n;++i){
      //printf("%d %d\n", i, mask);
    if(!(t[arrayNum] & mask))
      printf("%d\n",i);

    mask <<= 1;
    if (mask == 0)
    {
      ++arrayNum;
      mask = 1;
    }
  }
  diff = clock() - start;

  int msec = diff * 1000 / CLOCKS_PER_SEC;
  printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
  free(t);
  return 0;
}
