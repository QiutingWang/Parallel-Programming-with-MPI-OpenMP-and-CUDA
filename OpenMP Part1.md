# OpenMP Part1

## References:

- Guide into OpenMP: Easy multithreading programming for C++. <https://bisqwit.iki.fi/story/howto/openmp/>
- LLNL OpenMP Tutorial. <https://hpc-tutorials.llnl.gov/openmp/>
- Source Codes <https://github.com/EricDarve/cme213-spring-2021/tree/main/Code/Lecture_04>
- <https://www.cs.wm.edu/~xl10/cs312/slides/lecture16.pdf>


## 1. Why OpenMP?

- C++ threads are too general and complicated for engineering. It is suitable for low-level multi-core programming.
- OpenMP is a standard and simplified approach

## 2. Definition of OpenMP

- Two Expression Forms:
  - For loop: divide loop into blocks, each thread process one block
  - Identify a block of code which can concurrently run with the rest of code called task to a separate spread
- a kind of API which is a way to write multithread program and explain the meaning of syntax or directives that is defined by hardware/software vendor and national labs, a kind of *solution stack*
  - hardware fundamental structure: shared address space and multiple processors
    - Two types of shared memory computer:
      - Symmetric multiprocessor(SMP): equal time access to each processor
      - Non-Uniform address space multiprocessor(NUMA): 
        - different memory regions have different access costs
        - segment memory into near or far
  - System layer: OpenMP runtime library, system support for shared memory and threading
  - Program Layer: directives, complier, OpenMP library, Environment variables
  - User Layer: Application, End User
- With shared address model, threads communicate with shared variables

## 3. Coding

### Core Syntax

- STL: `#include <omp.h>`
- Complier directives format: `#pragma omp construct [clause[clause]...]`
- OpenMP constructs apply to a structured block with one top point of entry, one or more statements, and one point of exit at the bottom.
- Complier and Flag:
  - `gcc -fopenmp fileName` (frequently use)
    - for bash shell `export OMP_NUM_THREADS=4`
    - `./a.out` to run the program
  - `g++ -fopenmp fileName` (frequently use)
  - `g77 -fopenmp`
  - `gfortran -fopenmp`
  - `icc -openmp`
  - `icpc -openmp` 
  - `ifort -openmp`


### Installation

- `brew install libomp` or `brew install gcc`
- check the version: `grep OPENMP`

### Parallel Region Creation

- create a parallel region with default number of threads: `#pragma omp parallel{...}`
  - Request the specific number of threads: `pragma omp parallel num_threads(4)`
  - fork-join model
- Runtime lib for getting thread ID: `int ID=omp_get_thread_num();`
- Shared or Private variables
  - if data is allocated outside the pragma omp parallel that data set in a heap, and it is visible for and **shared** with all the threads
  - if data is allocated inside the pragma omp parallel, the data is set in an individual stack, which mean the data is **private** to that thread.

### Core Features of OpenMP

- Model: **Fork-Join** Parallelism
  - Master Thread(A single thread) produces a team of threads as needed with ID 0,1,...n-1 work in parallel
  - when they finish the work, they join back together again.
  - the *sequential program* evolves into a parallel program
  - *nested thread* can be used
- Complier:
  - interpret the codes of OpenMP with *pthreads* and *thunk*

### Synchronization

- Used to impose order constraints and protect access to shared data
- Common forms:
  - Barrier
    - `#pragma omp barrier`
    - threads wait at a barrier until all threads arrive
    - all threads are released at once and begin to execute code beyond the barrier.
    - code implements a barrier is a `critical section`.
      - execute a critical section:
        - lock->execute the code->release the lock
        - 0: lock is available; 1: lock is unavailable
  - Critical
    - `#pragma omp critical`
    - *only one thread* at a time can enter a critical region
    - order threads access to shared data
    - provide mutual exclusion
  - Atomic
    - `#pragma omp atomic`
    - provides mutual exclusion but only effects on the **update** to a memory location.


### For Loop

- creation: `#pragma omp parallel for`
- Scheduled in different ways `#pragma omp parallel for schedule(kind, chunk_size)`
  - it decides how loop iterations are mapped onto threads
  - Different Kinds
    - static
      - chunk size is fixed
      - threads work with the same speed, low overhead
    - dynamic
      - each thread gets chunk iterations from a *queue* until all iterations have been handled.
      - threads work with different speed, low overhead
      - the last thread will terminate long after others
    - guided
      - schedule and chunk size are gained from `OMP_SCHEDULE` environment variable
      - chunk size is different 
      - each continuous chunk is smaller than last one.
      - high overhead
    - auto
      - schedule is left to runtime library to choose
  - get/set with library routines: `omp_set_schedule()` and `omp_get_schedule()`
- the iteration index is *private* by default
- We need to make the loop iterations <u>independent</u> with the codes outside the parallel region
- Nested Loop:
  - `#pragma omp parallel for collapse(n)`, where `n` is *the number of loops* to be parallelized
- Reduction
  - `#pragma omp parallel for reduction(op: list)`
    - the variables in the `list` must be shared in the whole parallel region
  - it is used when we cannot split the loop iteration dependently with a **accumulative variable**(s)
  - we make **local copy** of each variable in list initialized depend on `op`. 
    - `op`: operands运算元, initial-values
    - eg: 0 for +, 1 for *, 0 for -...
    - The <u>updates</u> occur on local copy.
    - Local copies are reduced into a *single value* and related with *original global value*.
- nowait
  - `#pragma omp parallel for nowait`
  - the same schedule is used in two loops
  - threads completing assigned work can proceed without waiting for all threads in the team to finish.
  - avoid synchronization.

### Notes

- Mathematical Method to $\pi$ Calculation
  - Formula: $\int_0^1\frac{4}{1+x^2}dx=\pi$
  - Approximate the integral as the sum of rectangles:
    - $\sum_{i=0}^NF(x_i)\Delta x≈\pi$
      - the height $F(x_i)$ at the middle of interval $i$
      - width=$\Delta x$
