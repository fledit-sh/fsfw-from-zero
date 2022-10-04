# Inter-Process Communication (IPC) with the FSFW

IPC is a necessary tool to let software entities communicate with each other. In general, for
any Software, there are two primary ways for software entities to communicated with each other:

 1. Shared Memory. If memory is shared between threads or tasks, memory access needs to be protected
    with a lock, also commonly called Mutex.
 2. Message Passing. Usually, OSes provide some way of passing messages between threads safely.

Ín this workshop, we will look at both available ways to perform IPC with the FSFW.

# 1. Sharing state between two threads

## Subtasks
