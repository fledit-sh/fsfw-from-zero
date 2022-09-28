# Threads and Tasks

A satellite is a complex system which usually has a lot of tasks which need to be done
simulatenously by a dedicated On-Board Computer (OBC). This can include for example:

- TMTC handling. This includes Telecommand (TC) reception and execution, and the (autonomous)
  generation of Telemetry (TM)
- Control Operations, for example execution of the Attitue Control System (ACS) loop
- Handling of connected physical devices like sensors or payloads

Usually, these systems oftentimes have soft and even hard real-time requirements where longer delays
are not allowed and the system has an upper bound for response times.

This basically means that any software which does multiple non-trivial tasks needs a
(real-time) operating system to perform multiple tasks consecutively, with deterministisc
guarantees that these tasks are performed within a certain temporal bound.

Some common operating system in the Space domain able to do this:

- FreeRTOS for smaller MCUs (SOURCE)
- Embedded Linux (EIVE)
- RTEMS (FLP)

All these operating system use threads or tasks as the basic worker unit which is executing code.
This chapter first introduces threads as they are exposed by the C++ standard library.
After that, the code is transitioned to use the abstraction provided by the framework.

## 1. Scheduling a basic task using the C++ `std::thread` API

The goal of this task is to set up a basic thread which prints the following
string every second: "Executing Dummy Task".

 - [std::thread API](https://en.cppreference.com/w/cpp/thread/thread)
 - [Delaying a thread](https://en.cppreference.com/w/cpp/thread/sleep_for)

## 2. Changing to the concept of executable objects

Threads generally expect a function which is then directly executed.
Sometimes, the execution of threads needs to be deferred. For example, this can be useful
if the execution of tasks should only start after a certain condition.

Also, it might become useful to model any task in form of a class. An instantiation
of that class would then be an executable object. This is precisely what the framework
exposes in form of the [`ExecutableObjectIF`](https://documentation.irs.uni-stuttgart.de/fsfw/development/api/task.html).

It also offers a unform API to execute periodic tasks in form of the
[`PeriodicTaskIF`](https://egit.irs.uni-stuttgart.de/fsfw/fsfw/src/branch/master/src/fsfw/tasks/PeriodicTaskIF.h).

These tasks can then be created using the
[`TaskFactory`](https://egit.irs.uni-stuttgart.de/fsfw/fsfw/src/branch/master/src/fsfw/tasks/TaskFactory.h) singleton.

An arbitrary number of executable objects can then be passed to a periodic task. These objects
are then executed sequentially. This allows a granular design of executable tasks.
For example, important tasks get an own dedicated thread while other low priority objects are
scheduled consecutively in another thread.

The goal of this task is to convert the code from task 1 so the [std::thread]
API takes an executable object to move to a more object oriented task approach. 
The printout of the threaf should remain the same.

It is recommended to pass this executable object into the [std::thread] directly.

 - [std::reference_wrapper](https://en.cppreference.com/w/cpp/utility/functional/reference_wrapper)
    to pass referneces to the [std::thread] API.

As a bonus task, you can make your executable object implement a
[MyExecutableObjectIF] interface class. An interface class is
an [abstract class](https://en.cppreference.com/w/cpp/language/abstract_class) which
only contains pure virtual functions. As such, it can only be implemented by other
objects and describes a certain API contract an object has to fulfill.

## 3. Using the framework abstractions

As described before, the framework provides task abstraction with some advantages

- Task execution can be deferred until an explicit `start` method is called
- Same uniform API for multiple operating systems

The goal of this task is to implement the task specified in 1 using the
abstractions provided in step 1.
