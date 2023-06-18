# OpenMP Part2

## Reference

- Learning C Programming. <https://www.programiz.com/c-programming>
- Computers in Engineering—C Dynamic Memory Allocation. <https://user.engineering.uiowa.edu/~cie/LectureNotes/Lecture8.pdf>
- Introduction to OpenMP YouTube. <https://www.youtube.com/watch?v=Q96sRpbKzEs&list=PLLX-Q6B8xqZ8n8bwjGdzBJ25X2utwnoEG&index=23&t=310s>
- <https://indico.fysik.su.se/event/6743/contributions/10363/attachments/4168/4793/pdc19_OpenMP_intro.pdf>
- Shared Memory and OpenMP. <https://cims.nyu.edu/~stadler/hpc17/material/ompLec.pdf>

## 1. Work Sharing Construction

- Master Construct

  - Definition: a structured block that is only executed by the `<u>`master thread `</u>`
  - While, other threads just *skip* it, no synchronization is needed.
  - `#pragma omp parallel master`
  - if we want to capture the result of the master applied, set others to wait, we need a *barrier*.
  - eg:

  ```cpp
  #pragma omp parallel
  {
    do_many_things(); 
  #pragma omp master
  {  //usually thread0 does this
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
  #pragma omp parallel
  {
    do_many_things(); 
  #pragma omp single
  { 
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
  #pragma omp parallel{ //create a team of threads
    #pragma omp sections
    { 
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
  for(i=0; i<NBuckets; i++){ //build the histogram and set up the lock to each bin.
    omp_init_lock(&hist_locks[i]);
    hist[i]=0;
  }
  #pragma omp parallel for  //go through and sample some data which bin we want to reach and update the bin of histogram
  for(i=0; i<NVals; i++){  
    ival=int sample(arr[i]);
    omp_set_lock(&hist_locks[ival]);
    hist[ival]++; //update
    omp_unset_lock(&hist_locks[ival]); //release the lock and make sure mutual exclusion
  }
  for(i=0; i<NBuckets; i++){
    omp_destroy_lock(&hist_locks[i]); //free up the storage
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
    - `shared`
    - `private`: create a <u>local copy</u> of the variable for each thread
      - the value original variable is non-changed
      - the value of copied private variable is *uninitialized*.
    - `firstprivate`: variables gain *initialized values* from shared variable.
    - `lastprivate`:
      - the last value of private inside a parallel loop -> shared variable outside the loop
      - Variables **updates** shared variable using value from *last iteration*
      - C++ objects are updates by assignment (`=`)
    - `default`
      - the default storage attribute is `default(shared)`, so we do not need to state it once again, except `#pragma omp task`
      - `default(private)`
        - each variable in the construct is private
        - only *Fortran* supports this code.
      - `default(none)`
        - no default for variables. Hence, we need to specify each variable data sharing attribute.
        - C++ only supports `default(none|shared)`
    - `linear`
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
  - task variables are *private* by default which are undefined out of the task construct
  - Solution:

  ```cpp
  #include <omp.h>
  #include <iostream>
  using namespace std;

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

- Depend
  - Syntax: `#pragma omp task depend(depend_type, variable_list)`
    - depend types:
      - `in`: input variables
      - `out`: output variables
      - `inout`: input & output variables
      - muteinoutset:define mutually exclusive tasks
  - Role: to define whether tasks are mutually independent or related to each other
- Priority: 
  - help to schedule the tasks in a more optimal execution order. Higher priority, sooner the task will be run.
  - Syntax: `#pragma omp task priority(priorityValue)`
    - higher priority value stated, higher priority.
- Taskloop:
  - Syntax: 

  ```c
  #pragma omp taskloop [clause[[,] clause] ...] new-line
  for(){

  }
  ```

  - specifies the iterations of the loops will be executed in **parallel** using tasks.
- Taskgroup:
  - Tasks and their descendent tasks can be synchronized by including in taskgroup block
  - specifies a wait on completion of child tasks of the current task and their descendent tasks
  - All tasks generated inside a `taskgroup` region are waited for at the end of the region.
  
```c
  #include <stdio.h>
  #include <omp.h>
  int main(){
    #pragma omp parallel
    #pragma omp single nowait
    {
      #pragma omp taskgroup
      {
        #pragma omp task
        {
          #pragma omp task
          printf("Hello.\n");
          printf("Hi.\n");
        }
      }
      printf("Goodbye.\n");
    }
    return 0;
}
```

Return:

```
Hello.
Hi.
Goodbye
```

## 6. OpenMP Memory Model

- OpenMP supports shared memory model. All threads share an address space.
- Determination Factors:
  - Coherence(cache):
    - what happens to memory system when <u>single</u> address is accessed by multi-threads
    - propagate new values to other cached copies
  - Consistency
    - Definition:
      - constraints on orders of write(W), read(R), synchronization(S) with multiple threads for all shared-memory address
      - Reordering: complier reorders `program order` to `code order`; machine reorders code order to `memory commit order` (coding order != executable order != memory order)
      - how do values seen by a thread change as we change the order of operations(RSW)
    - Sequential consistency
      - program order=code order=memory commit order
      - remain in *program order* for each processor
    - Relaxed consistency
      - remove some ordering constraints for memory operations.
      - The threads do not need to update data in any specific order.
      - each thread has its own temporary view of the values of shared data
    - OpenMP supports Weak consistency
      - cannot reorder S ops with R or W on the same thread
      - S ops are related to flush.

## 7. Flush

- Definition:
  - flush set:
    - **all** threads variables are visible for a flush construct *without* argument list
    - a set of variables when using `flush` with a list.(not recommended)
  - Role: used to update data in memory, all other threads see the newest and same value, so that threads can pick up the right value
  - flush with overlapping flush sets cannot be *reordered*
  - it can keep tracking of consistency
  - all R,W ops that overlap the flush set:
    - occur prior to flush complete before flush executes.
    - occur after flush don't execute until after the flush
- Syntax: `#pragma omp parallel flush [(list)]`
- Flush and Synchronization
  - flush is implied by synchronization, the locations: entry/exit of parallel/critical regions, barriers, lock set or unset
  - Producer consumer pattern:
    - the output of one thread produces is the value that another thread consumes
    - often use a *stream* of produced values to implement 'pipeline parallelism'
  - implement and build <u>pairwise synchronization</u> between threads
    - define a shared flag variable
    - reader spins waiting for new flag value, checking the value of flag
    - use *flush* to force updates to and from memory
    - pitfalls: program race, solution: atomics

## 8. Atomics

- Function: 
  - ensure a specific storage location is accessed **atomically**(single thread), instead of exposing to multiple, simultaneous R and W threads leading to indeterminate values.
  - avoid race conditions
- Syntax: `#pragma omp atomic [read|write|update|capture]`
  - protect loads: `#pragma omp atomic read` `var=x`
  - protect stores: `#pragma omp atomic write` `x=expr`
  - protect updates to a storage location: `#pragma omp atomic update` `x++; ++x; x--; --x; x binop=expr; x=x binop expr`
    - `binop`: one of +,*,-,/,&,^,|,<<, or>> .
  - protect assignment of a value and an associated update operation: `#pragma omp atomic capture`
    - statements
      - `var=x++; var=x--; var=++x; var=--x; var=x binop=expr`
    - structured blocks:

      ```c
      {var=x; x binop=expr;} 
      {x binop= expr; var = x;} 
      {var=x;x=x binop expr;} 
      {x=x binop expr;var=x;} 
      {var = x; x++;}
      {var = x; ++x;}
      {++x; var = x;}
      {x++; var = x;}
      {var = x; x--;}
      {var = x; --x;}
      {--x; var = x;}
      {x--; var = x;}
      ```

## 9. Threadprivate Data

- Data sharing: threadprivate
  - Function and Role: 
    - make global data private to a thread
      - objects: file scope variable(visible for all), static variables, static class members
    - != make them `private`
      - `private`: mask global variable
      - `threadprivate`: preserve global scope in each thread
    - initialization: use `copyin(list)` or at time of definition
      - each copy of threadprivate variable is initialized *once*.
    - specify variables are replicated, each thread has its own copy
  - Syntax: `#pragma omp threadprivate(variable_list)`
  
  ```c
  int counter=0; //initialize as the time define
  #pragma omp threadprivate(counter);
  int increment_counter(){
    counter++;
    return counter; //return counter per thread
  }
  ```

- Data copying: copyprivate
  - Roles: used a *single* region to spread values of private from one member of team to the rest of the team before reaching to barrier

