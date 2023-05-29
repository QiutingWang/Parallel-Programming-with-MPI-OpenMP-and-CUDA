# OpenMP Part2


## 1. Work Sharing Construction

- Master Construct

  - Definition: a structured block that is only executed by the `<u>`master thread `</u>`
  - While, other threads just *skip* it, no synchronization is needed.
  - `#pragma omp parallel master`
  - if we want to capture the result of the master applied, set others to wait, we need a *barrier*.
  - eg:

  ```cpp
  #pragma omp parallel{
    do_many_things(); 
  #pragma omp master{  #usually thread0 does this
    exchange_boundaries(); 
    } 
  #pragma omp barrier
  do_other_things(); 
  }
  ```
- Single work Sharing construct

  - Definition: a block of code which is executed by *only one thread*, not necessarily the master thread
  - `pragma omp single`
  - a `barrier` is effective at the end of single block **impliedly**. If we want to remove barrier, a `nowait` clause is needed.
  - eg:

  ```cpp
  #pragma omp parallel{
    do_many_things(); 
  #pragma omp single{ 
    exchange_boundaries(); 
    } 
    do_other_things();
    }
  ```
- Sections work sharing construct

  - Definition: gives a different structured block to each thread. One thread does each of sections.
  - `pragma omp sections`
  - eg:

  ```cpp
  #pragma omp parallel{ #create a team of threads
    #pragma omp sections{ 
      #pragma omp section
        X_calculation();
      #pragma omp section
        Y_calculation();
      #pragma omp section
        Z_calculation();
    }
  } 
  # By default, there is a `barrier` at the end of `omp sections`. And use `nowait` clause to turn off the barrier.
  ```

## 2. Lock Routines

- STL: `#include <omp.h>`
- Why we need lock:
  - a lowest level of mutual exclusion synchronization in concurrency.
- Simple Lock routines
  - `omp_init/set/unset/test/destroy_lock()`
    - test: to check whether the lock is available
  - simple lock is available if it is *unset*.
- Nested Locks
  - `omp_init/set/unset/test/destroy_nest_lock()`
  - nested lock is available when it is *unset* or it is *set* but owned by the thread which executing *nested lock function*.
- Example:
  
  ```cpp
  #pragma omp parallel for
  for(i=0; i<NBuckets; i++){ #build the histogram and set up the lock to each bin.
    omp_init_lock(&hist_locks[i]);
    hist[i]=0;
  }
  #pragma omp parallel for  #go through and sample some data which bin we want to reach and update the bin of histogram
  for(i=0; i<NVals; i++){  
    ival=int sample(arr[i]);
    omp_set_lock(&hist_locks[ival]);
    hist[ival]++; #update
    omp_unset_lock(&hist_locks[ival]); #release the lock and make sure mutual exclusion
  }
  for(i=0; i<NBuckets; i++){
    omp_destroy_lock(&hist_locks[i]); #free up the storage
  }
  ```

## 3. Runtime Library Routines

- Set up and check number of threads:
  - `omp_set_num_threads()`:request to use certain number of threads
  - `omp_get_num_threads()`: capture how many threads do we actually have
  - `omp_get_thread_num()`: return thread's ID
  - `omp_get_max_threads()`: return an integer => numbers of threads that would be available.
- Check whether we are in an active parallel region:
  - `omp_in_parallel()`
- Check number of processors in the system: `omp_num_procs()`
- Dynamically vary the number of threads from one parallel construct to another:
  - `omp_set_dynamic()`: if we put 0 in the bracket, we disable number of threads dynamic adjustment. 
  - `omp_get_dynamic()`: check whether we are in dynamic mode or not

## 4. Data Environment

- Data Sharing: data storage attributes applied to <u>parallel constructs</u>, not the whole region, except *shared*.
  - syntax: `#pragma omp parallel storageAttributeType(variable_list)`
  - Types
    - shared
    - private: create a local copy of the variable for each thread
      - the value original variable is non-changed
      - the value of copied private variable is *uninitialized*.
    - firstprivate: variables gain initialized values from shared variable.
    - lastprivate: 
      - the last value of private inside a parallel loop -> shared variable outside the loop
      - Variables **updates** shared variable using value from *last iteration*
      - C++ objects are updates by assignment (`=`)
    - default
      - the default storage attribute is `default(shared)`, so we do not need to state it once again, except `#pragma omp task`
      - `default(private)`
        - each variable in the construct is private
        - only *Fortran* supports this code.
      - `default(none)`
        - no default for variables. Hence, we need to specify each variable data sharing attribute.
        - C++ only supports `default(none|shared)`
    - linear
      - define >=1 variables to be private and have a linear relationship with the iteration space of a loop related to parallel constructs where the clause appear


## 5. Task

- `#pragma omp task`
- Definition: independent units of work
- Composition:
  - code, data environment, internal control variable
- Runtime decides when to execute tasks, immediately or deferred.
- Tasks are ensured to *be complete* at thread barrier or task barrier:
  - `pragma omp barrier`
  - `pragma omp taskwait`
- Data Scoping Problem:
  - task variables are private by default which are undefined out of the task construct
  - Solution:
  ```cpp
  int fib(int n){
    int x,y;
    if(n<2){
      return n;
    }
    #pragma omp task shared(x)
    x=fib(n-1);
    #pragma omp task shared(y)
    y=fib(n-2);
    #pragma omp taskwait
    return x+y;
  }
  int main(){     
    cout<< fib(6);
    return 0;
  }
  ```

## 6. OpenMP Memory Model

- OpenMP supports shared memory model. All threads share an address space.
- Determination Factors:
  - Coherence:
    - what happens to memory system when single address is accessed by multi-threads
  - Consistency
    - Definition: 
      - constraints on orders of write, read, synchronization with multiple threads
      - Reordering: complier reorders `program order` to `code order`; machine reorders code order to `memory commit order`
      - how do values seen by a thread change as we change the order of operations(RSW)
    - Sequential consistency
      - program order=code order=memory commit order
    - Relaxed consistency
      - remove some ordering constraints for memory operations.
- Flush


