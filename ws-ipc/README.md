# Inter-Process Communication (IPC) with the FSFW

IPC is a necessary tool to let software entities communicate with each other. In general, for
any Software, there are two primary ways for software entities to communicate with each other:

 1. Shared Memory. If memory is shared between threads or tasks, memory access needs to be protected
    with a lock, also commonly called Mutex.
 2. Message Passing. Usually, OSes provide some way of passing messages between threads safely.

Ín this workshop, we will look at both available ways to perform IPC with the FSFW.
I recommend to read the thread and mutual exclusion chapters of the
[the concurrency chapter provided by the isocpp](https://isocpp.org/wiki/faq/cpp11-library-concurrency)
if you are completely new to concurrency in C++.

# 1. Sharing state between two threads

## Subtasks

1. Create two threads which run some tasks with a period of 50ms using the
   [`std::thread`](https://en.cppreference.com/w/cpp/thread/thread) API
2. Introduce a static global `uint32_t` variable called `SHARED_VARIABLE`
3. Increment the variable in both threads, but ensure that the access
   is protected by a [`std::mutex`](https://en.cppreference.com/w/cpp/thread/mutex).
   You can also use the [`std::lock_guard`](https://en.cppreference.com/w/cpp/thread/lock_guard)
   which is a [RAII-style](https://en.cppreference.com/w/cpp/language/raii) helper object.

# 2. Using messages

C++ does not really have an built-in message queue implementation.
We are going to use a `std::queue` in conjunction with a `std::mutex` to
have something similar to a message queue API.

##