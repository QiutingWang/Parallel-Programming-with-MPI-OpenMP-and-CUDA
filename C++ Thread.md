# C++ Thread

## 1. Process

- Definition: program in execution
- Composition: executable program + necessary information

## 2. Thread

- Definition:
  - extension of process model, regarded as _lightweight_ process
  - a _procedure_ runs independently from main program
  - each process might consist of multiple independent <u>control flows</u> which run simultaneously

## 3. Shared Address Space

- All threads of one process share the common address space.
- If one thread store a value in the shared address space, other threads of the same process can access this value as well.
- As a data exchange area

## 4. C++ Threads

- C++: STL `#include <thread>`
  - Reference:
    - Lecture Source Codes: <https://github.com/EricDarve/cme213-spring-2021/tree/main/Code/Lecture_03>
    - 🌟Back to Basics: Concurrency - Mike Shah - CppCon 2021. <https://www.youtube.com/watch?v=pfIC-kle4b0>
    - <https://en.cppreference.com/w/cpp/thread/thread>
  - Allow multiple functions to execute concurrently.
- Python: `import threading`
  - Reference:
    - Multiprocessing vs. Threading in Python: What Every Data Scientist Needs to Know. <https://blog.floydhub.com/multiprocessing-vs-threading-in-python-what-every-data-scientist-needs-to-know/>
    - <https://git-cheat-sheets.readthedocs.io/en/latest/threading.html>

- Basic operations:
  - constructor: `thread name(function, args);`
    - other can be passed into thread constructor: lambda expression, pointer to a function
  - with Reference Argument: `thread name(function, ref(args));`
    - caused by passed by reference of objects
  - Empty thread cannot be associated with `.detach()` `.join()`
  - `name.get_id()`
  - threads join: `name.join();`
    - `.joinable()` be true if we can do `join()`
    - wait for the thread to finish its execution then comes to main thread
    - join with main thread
- Threads in Collection:

  - if we would like to launch multiple threads at a time, we can use a vector of threads then loop it. `#include <vector>;`

    ```
      for(int i=0; i< numbers of threads; i++){
        threads.push_back(thread(function, i));
        threads[i].join();
      }
    ```

    - While, it automatically get our program runs sequentially without performance gain.

  - Another Solution:

    ```
    for(int i=0; i< numbers of threads; i++){
       threads.push_back(thread(function, i));
       }
    for(int i=0; i< numbers of threads; i++){
       threads[i].join();
       }
    ```

    - This method makes the program runs out of order.

- **jthread**
  - launches the thread and joins thread on destruction
  - ```
    jthread name;
    for(int i=0; i< numbers of jthreads; i++){
        threads.push_back(jthread(function, i));
        }
    ```
- **Allocate Memory Resource and Query the Returns**:
  - Reference: <https://cplusplus.com/reference/future/promise/>
  - use `promise` and `future`
    - A promise is an object can store the value retrieved by future object. They are used as a pair.
    - promise: asynchronous provider `promise<dataType> name;`, set value only once
    - future: asynchronous return object `future<dataType> name;`, get value only once
  - STL: `#include <future>`
  - Operations:
    - `promise<dataType> promiseName;`
    - `future<dataType> futureName=promiseName.get_future();`
    - `promiseName.set_value(value);`
    - `thread name3(function, move(promiseName));`
    - `futureName.get();`
    - `futureName.valid()`: checks if the future can be used
    - `futureName.wait()`: waits for the result to become available
    - only `futureName.
    - one-to-many: `shared_future<int> sfName = promise.get_future().share();`
      - allows multiple getting
- **Race Condition/Data Race**:
  - the risk of multi-threaded programming concurrency
  - the final result depends on <u>non-deterministic order</u> in which the instruction executed write and read.
- **Mutex**
  - two states: locked/ unlocked
    - if a thread locks a mutex:
      - nobody else can access to the mutex
      - only the thread initially locked the mutex has the ability to unlock it
  - `#include <mutex>` <https://en.cppreference.com/w/cpp/thread/mutex>, mutual exclusion to a block of code
  - used to protect shared data from being accessed by multiple threads at the same time.
  - only _one thread_ can execute the code at a time(Atomic)
  - Operations:
    - constructor: `mutex lockName;` create a global lock
    - `lockName.lock()`: blocking operation, the critical section is protected by the lock
    - `lockName.unlock()`
    - `lockName.try_lock()`: locking without blocking operation. Return true if the lock has been acquired
    - `lock_guard<mutexType> lockGuard(lockName)`: take ownership of the lock, use RAII to release the lock, prevent us from forgetting to unlock a lock
      - the destructor is `.unlock()`
- **Deadlock**:
  - happens when the lock is never returned, a thread doesn't release a lock, goes out of the scope before releasing the lock
  - it is an extreme form of starvation, which means thread cannot fairly access to a resource. Lack of further progress.
  - Solution:
    - `thread sanitizer`to detect deadlock before running the program
    - `unique_lock`+`lock`: `unique_lock<mutex> lock<lockName>;`, delay blocking, cooperated with condition variables, follow RAII
    - `scoped_lock`: can obtain multiple locks at once, which is preferred over lock_guard.
- **Atomic**:
  - STL: `#include <atomic>;`
  - atomic variables ensure write and read synchronizes
  - `atomic<dataType> variableName=value;`
  - operation:
    - `store()`: save values in a atomic variable
    - `load()`: read values from atomic variable
- **Condition Variable**

  - STL: `#include <condition_variable>`
  - it allows us to take one thread off the safe queue, until it is notified to start working and no longer need to wait.
  - Constructor: `condition_variable CVName;`
  - operations:
    - `CVName.wait()`: waiting for a change
    - `CVName.notify_one()`: awakens one reporter thread waiting for change. But we have no control over which one to specified.
    - `CVName.notify_all()`: awakens all threads waiting for change

- **Auxiliary functions**:
  - thread identification: `std::this_thread::get_id()`
  - put thread into sleep:
    - STL: `#include <chrono>`
    - `std::this_thread::sleep_until(const chrono::time_point& sleep_time)`
    - `std::this_thread::sleep_for(const chrono::duration& sleep_duration)`
    - `std::this_thread::yield()`

## 5. Asynchronous Programming

- Definition: without order
- STL: `#include <future>;`
- construct a asynchronous function which can be treated as a future: `auto asyncFunction=async(function, argument);`
- `asyncFunction.get();`
- `asyncFunction.wait();`
