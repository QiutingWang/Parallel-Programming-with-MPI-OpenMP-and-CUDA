#include <omp.h>
#include <iostream>
#include <vector>
using namespace std;

//hello world
int main(){
  #pragma omp parallel{
    int ID=omp_get_threads_num(); // threads are indexed from thread0
    printf("Hello(%d)", ID);
    printf("world(%d)\n", ID);
  }
}

//pi with Calculus with PAD array
static long num_steps=100000;
double step;
#define PAD 8
#define NUM_THREADS 2

void main(){
  int i, nthreads;
  double pi, sum[NUM_THREADS][PAD];
  step=1.0/(double)num_steps;
  omp_set_num_threads(NUM_THREADS);
  #pragma omp parallel{
    int i, id, nthrds;
    double x;
    id = omp_get_thread_num(); 
    nthreads = omp_get_num_threads(); 
    if (id == 0) { //only one thread should copy the number of threads to global value that makes sure multiple threads access to the same address without any conflicts.
      nthreads = nthrds;
    }
    // Single Program Multiple Data(SPMD) design pattern
      // MPI programs use this pattern mostly
      // run the same program in n processing elements with rank id from 0,1,2,3...n-1
    for(i=id, sum[id]=0.0; i< num_steps; i=i+nthrds) { 
      x = (i+0.5)*step; //get median height
      sum[id][0]+=4.0/(1.0+x*x);
      }
  }
  for(i=0, pi=0.0; i<nthreads; i++){
    pi += sum[i][0]*step; //get the area under the mathmatical function curve
  }
}

// We can eliminate the False sharing by padding the sum array
  // each sum value contains in a different cache line
  // False Sharing: independent data elements sit on the same cache line, does the poor scaling 


// pi calculation with critical section to remove false sharing
static long num_steps=100000;
double step;
#define NUM_THREADS 2

void main(){
  int i, nthreads;
  double pi;
  step=1.0/(double)num_steps;
  omp_set_num_threads(NUM_THREADS);
  #pragma omp parallel{
    int i, id, nthrds;
    double x, sum;
    id = omp_get_thread_num(); 
    nthrds = omp_get_num_threads(); 
    if (id == 0) { 
      nthreads = nthrds;
    }
    id = omp_get_thread_num(); 
    nthrds = omp_get_num_threads(); 
    for(i=id, sum=0.0; i< num_steps; i=i+nthreads) { 
      x = (i+0.5)*step; 
      sum+=4.0/(1.0+x*x);
      }
  }
  #pragma omp critical
  pi += sum[i][0]*step; 
}


//pi calculation with atomic
static long num_steps=100000;
double step;
#define NUM_THREADS 2

void main(){
  double pi;
  step=1.0/(double)num_steps;
  omp_set_num_threads(NUM_THREADS);
  #pragma omp parallel{
    int i, id, nthrds;
    double x, sum;
    id = omp_get_thread_num(); 
    nthrds = omp_get_num_threads(); 
    if (id == 0) { 
      nthreads = nthrds;
    }
    id = omp_get_thread_num(); 
    nthrds = omp_get_num_threads(); 
    for(i=id, sum=0.0; i< num_steps; i=i+nthreads) { 
      x = (i+0.5)*step; 
      sum+=4.0/(1.0+x*x);
      }
  }
  sum= sum*step
  #pragma omp atomic
  pi += sum; 
}

/// Work Sharing
// Split up pathways between threads within a team
// Format:
  // Loop construct: splits up loop iterations among the threads within a team
  // Section construct
  // Single construct
  // Task construct

// pi calculation with for loop
static long num_steps=100000;
double step;

void main(){
  int i;
  double pi, x, sum=0.0;
  step=1.0/(double)num_steps;
  #pragma omp parallel{
    double x;
    #pragma omp for reduction(+:sum)
    for(i=0; i< num_steps; i=i++) { 
      x = (i+0.5)*step; 
      sum+=4.0/(1.0+x*x);
      }
  }
  pi = sum*step; 
}