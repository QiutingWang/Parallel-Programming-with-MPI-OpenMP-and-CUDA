#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>

///////////////////////////////////
/////Producer consumer pattern/////
//Define some random number constants 
#define N        10000
#define SEED       2531
#define RAND_MULT  1366
#define RAND_ADD   150889
#define RAND_MOD   714025
int randy = SEED;

// fill an array with random numbers
void fill_rand(int length, double *A){
  int i; 
  for (i=0; i<length; i++) {
    randy = (RAND_MULT * randy + RAND_ADD) % RAND_MOD;
    *(A+i) = ((double) randy)/((double) RAND_MOD);
    }  
}

// sum the elements of an array
double Sum_array(int length, double *A){
  int i;  
  double sum = 0.0;
  for (i=0; i<length; i++){
    sum += *(A+i);
  }  
  return sum; 
}
  
int main(){
  double *A, sum, runtime;
  int num_threads, flag=0;
  A=(double *)malloc(N*sizeof(double)); //alloc n bytes of memory //sizeof():Determines the size in bytes of a data type. A is the pointer as a return
  runtime=omp_get_wtime(); //returns elapsed wall clock time in seconds
  fill_rand(N,A); //producer
  sum=Sum_array(N, A) //consumer
  runtime=omp_get_wtime()-runtime;
  printf("In %f seconds, the sum is %f \n", runtime, sum);
  }

/////Pairwise Synchronization
int main(){
  double *A, sum, runtime;
  int num_threads, flag=0;   //flag is to signal at the time produced output is ready
  A=(double *)malloc(N*sizeof(double));
  #pragma omp parallel sections{
    #pragma omp section{
      fill_rand(N, A);
      #pragma omp flush //force to refresh memory, guaranteeing the newest value of A can be seen
      flag=1;
      #pragma omp flush(flag)
    }
    #pragma omp section{
      #pragma omp flush(flag)
      while(flag==0){
        #pragma omp flush(flag)
      }
      #pragma omp flush
      sum=Sum_array(N,A);
    }
  }
}