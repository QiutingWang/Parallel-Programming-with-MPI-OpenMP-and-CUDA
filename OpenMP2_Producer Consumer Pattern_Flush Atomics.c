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
  int num_threads, flag=0; //Flag = Synchronization
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
  #pragma omp parallel sections{ //one section to be the producer, another treated as consumer
    #pragma omp section{
      fill_rand(N, A);
      #pragma omp flush //check the update
      flag=1; 
      #pragma omp flush(flag) //make flag visible
    }
    #pragma omp section{
      #pragma omp flush(flag) //grab the current value of flag
      while(flag==0){  //keep spinning
        #pragma omp flush(flag) //go back to the memory cache to grab the newest flag value
      }
      #pragma omp flush //general flush is expensive, refresh the value of everything especially care about A
      sum=Sum_array(N,A);
    }
  }
}
// Programming race problem, solution: atomics. but it still works, because we only care about when we have flag==0 

/////Atomics and synchronization flags, race-free version
int main(){
  double *A, sum, runtime;
  int num_threads, flag=0, flg_tmp;   
  A=(double *)malloc(N*sizeof(double));
  #pragma omp parallel sections{
    #pragma omp section{
      fill_rand(N, A);
      #pragma omp flush 
      #pragma omp atomic write
      flag=1;
      #pragma omp flush(flag)
    }
    #pragma omp section{
      while(1){
        #pragma omp flush(flag)
        #pragma omp atomic read
        flg_tmp=flag;
        if(flg_tmp==1) break;
      }
    }
      #pragma omp flush
      sum=Sum_array(N,A);
  }
}
