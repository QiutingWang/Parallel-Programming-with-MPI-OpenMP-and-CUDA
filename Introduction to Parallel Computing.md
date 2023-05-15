# Introduction to Parallel Computing

## 1. Why Parallel Computing

- If we simply increase the number of transistor _to achieve faster computing speed_, there exists some problems:
  - leakage increases
  - power consumption and generated heat increases
  - Memory access time has not been reduced

## 2. Simple Introduction to Parallel Programming

- It have <u>multiple cores</u> on processor to compute and exchange data. We have two types of multi-core processors:
  - MultiCore != ManyCore
    - **ManyCore**: Many but Slower cores
      - normally used for *complex work*, such as optimization, document processing
    - **MultiCore**: One/Few but Fast cores, GPUs
      - normally it is used for *regular operations*, such as deep learning implement, matrix multiplication, math calculation
- Different from `Sequential` program, which needs to ask for the pivot in each step to know what is the next column to compute with
  - Task-based Parallel programming: each square or box is a bit of calculation with input data, the outgoing arrows are the data produced by this calculation and required by other boxes.
  
```
for(int i=0; i<n; i++){
  x=ComputeNextValue();
  sum += x;      //in this case, += needs us to do the inherence sequential mathematical operation
}
```

- The acceleration as follows:

```
int r; //declare the number of threads, the block number
int b; //declare the number of processed entity
int first_i=r*b; #beginning index
int last_i=(r+1)*b;
for(int i=first_i; i<last_i; i++){
  x=ComputeNextValue();
  sum += x;  //the addition is associated or commutative
}
```

- Working Procedure: 
  - Different processing agents can run the operation on data independently at the same time, then combine the partial results at the end to obtain the final answer.

- Another Solution: 
  - Procedure:
    - set `one master thread` to do all the work
    - loop other cores to send data and do the final addition sequentially
  - Problem: if there exist a few number of cores, the final addition will be time consuming.

## 3. Shared Memory Processor

- related to GPU computing
- Composition:
  - numbers of processors with caches which can temporarily store data, fast speed owing to the close physical distance
  - a **shared** physical global memory
    - any processor can read and write memory
  - interconnection network which connects the processors with memory
  - I/O

## 4. Shared Memory NUMA(Non-uniform Memory Access)

- treat memory as a *single addressable space*.
- this system has faster access to memory.
- the components are mostly the same as Shared Memory Processor, except the memory are block and close to cache. The general process is `(processor->cache->memory)*N->interconnection network<->I/O`
- ccNUMA(Cache coherent NUMA): interprocessor communication between multiple cache who share the same memory location.