# Threads and Tasks

This workshop is split into 4 subtasks which are done in the `main.cpp` of this
project.

## Background Information
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

- FreeRTOS for smaller MCUs (e.g. SOURCE CubeSat project)
- Embedded Linux (EIVE CubeSat project)
- RTEMS (FLP satellite project)

All these operating system use threads or tasks as the basic worker unit which is executing code.
This chapter first introduces threads as they are exposed by the C++ standard library.
After that, the code is transitioned to use the abstraction provided by the framework.

## 1. Scheduling a basic task using the C++ `std::thread` API

The goal of this task is to set up a basic thread which prints the following
string every second: "Hello World".

 - [std::thread API](https://en.cppreference.com/w/cpp/thread/thread)
 - [Delaying a thread](https://en.cppreference.com/w/cpp/thread/sleep_for)

## 2. Changing to the concept of executable objects

The goal of this task is to convert the code from task 1 so the `std::thread` API receives an
executable object to move to a more object oriented task approach.

The printout of the thread should remain the same. The executable objects should be named
`MyExecutableObject`. It contains one function called `periodicOperation` which performs the 
printout, and a static function which takes the `MyExecutableObject` itself by reference and
executes it in a permanent loop.

The executable object should be passed into the `std::thread` directly. There are
two ways to do this.

### Hints

 - [std::thread constructor](https://en.cppreference.com/w/cpp/thread/thread/thread)
 - [std::reference_wrapper](https://en.cppreference.com/w/cpp/utility/functional/reference_wrapper)
    to pass references to the [std::thread] API
 - [std::chrono::milliseconds](https://en.cppreference.com/w/cpp/chrono/duration) has a constructor
    where an `uint32_t` can be used to create the duration from a custon number.

### Subtasks

  1. Create a class called `MyExecutableObject` with a `public` block.
  2. Add a static function called `executeTask` which expects itself (`MyExecutableObject& self`) as
     a parameter with an empty implementation
  3. Add a regular method called `performOperation` which performs the printout
  4. Implement `executeTask`. This function uses the passed object and performs the scheduling
     specific part by calling `self.performOperation` in a permanent loop with a delay between
     calls. You can hardcode the delay to 1000ms for the first implementation.
  5. Change your `std::thread` calls in the main. You can pass the new `executeTask` function
     as the executable unit. The second argument should be an instance of the executable object
     itself. You might need the `std::reference_wrapper` to pass it as a reference. You can also
     use the approach used in
     [thre thread ctor example (t5)](https://en.cppreference.com/w/cpp/thread/thread/thread)
     and pass a regular member function and a pointer of the class
  6. Add a constructor to `MyExecutableObject` which expects a millisecond delay
     as an `uint32_t` and cache it as a member variable. Then use this member
     variable in the `executeTask` implementation to make the task frequency configurable via the
     constructor (ctor) parameter.

With the conversion to executable object, we have reached a useful goal in object-oriented
programming (OOP) in general: The application logic inside `performOperation` is now decoupled
from the scheduling logic inside `executeTask`. This is also called seperation of concerns.

## 3. Making the executable objects generic

Threads generally expect a function which is then directly executed.
Sometimes, the execution of threads needs to be deferred. For example, this can be useful
if the execution of tasks should only start after a certain condition.

Also, it might become useful to model any task in form of a class. An instantiation
of that class would then be an executable object. Another point is that even though
we have seperated the scheduling specific part from the application logic, they are still
part of the same class. It would be nice to have two separate classes for this.

C++ as an OOP language provides abstraction in form of interfaces, which can be used to have
different types of generic executable objects. Interfaces usually do not have a lot of source code
on their own. They describe a design contract a class should have which implements the interface.
In general, the FSFW relies heavily on subclassing and inheritance to provide adaptions point to users.

We are going to refactor our `MyExecutableObject` by introducing an interface for any executable
object. We are then going to add a generic class which expects an object fulfilling this design
contract and then executes that object.

Interfaces in C++ are implemented using
[abstract classes](https://en.cppreference.com/w/cpp/language/abstract_class) which only contains
pure virtual functions.

### Subtasks

 1. Create an interface called `MyExecutableObjectIF`. You can create this like a regular class.
    As opposed to Java the differences between interfaces and classes are only by convention.
 2. In general, it is recommended to add a virtual destructor to an interface. It looks like this:
    ```cpp
    virtual ~<Class>() = default;
    ```

 3. Add a abstract virtual function `performOperation`.
    Abstract virtual functions look like this in general

    ```cpp
    virtual <functionName>(...) = 0;
    ```

 4. Implement you custom interface for `MyExecutableObject` by re-using the exsiting
    `performOperation` function. In general, when implementing
    an interface or overriding a virtual function, it is recommended to add the `override` keyword
    to the function delaration. We do not have seperation between source and header files for
    our class yet, so you can add the `override` keyword after the function arguments and before
    the implementation block. The compiler will throw a compile error if a function is declared
    override but no base object function was actually overriden. This can prevent subtle bugs.
    Please note that `MyExecutableObject` is actually now forced to implement the
    `performOperation` function because that function is pure. The compiler makes sure we fulfill
    the design contract specified by the interface
 5. Add a new class called `MyPeriodicTask`. Our executed object and the task abstraction
    are now explicitely decoupled by using composition. Composition means that we have
    a "has-a" relationship instead of a "is-a" relationship. In general, composition is preferable
    to inheritance for flexible software designs. The new `MyPeriodicTask` class should
    have a ctor which expects a `MyExecutableObjectIF` by reference and the task frequency in
    milliseconds as an `uint32_t`. It caches both the executbale object and the task frequency
    as private member variables.
 6. Add a public `start` function which returns a `std::thread` and leave it empty for now.
 7. Add a private static `executeTask` method which expects `MyPeriodicTask` by reference.
    Its implementation is similar to the `executeTask` method of `MyExecutableObject`.
    Remove the `executeTask` implementation from `MyExecutableObject`.
 8. In the start method, use `std::thread` API with `MyPeriodicTask::executeTask` as the
    executed function. Pass the task itself by reference similarly to how it was done in task 2.
    Return the created thread directly, so callers can use the `join` method to block on thread
    completion.

We now have two separate classes where one class only contains application logic and the other
one only contains scheduling logic. The `MyPeriodicTask` is also able to schedule arbitrary
types which implement `MyExecutableObjectIF`. Finish this task
by crating 3 different executable objects where each object does or prints our something
different. Then pass all of those three different objects to a `MyPeriodicTask` and start
all three periodic tasks which three different frequencies.

You now should have code which looks something like this:

```cpp
int main() {
    MyExecutableObject0 myExecutableObject0;
    MyExecutableObject1 myExecutableObject1;
    MyExecutableObject2 myExecutableObject2;
    MyPeriodicTask task0(myExecutableObject0, 1000);
    MyPeriodicTask task1(myExecutableObject1, 2000);
    MyPeriodicTask task2(myExecutableObject2, 5000);
    auto thread0 = task0.start();
    auto thread1 = task1.start();
    auto thread2 = task2.start();
    thread0.join();
    thread1.join();
    thread2.join();
    return 0;
}
```

Where the three tasks do their tasks with different frequencies.

## 4. Using the framework abstractions

We now use framework components to perform the tasks shown above. The framework
exposes an abstractions for executable tasks called [`ExecutableObjectIF`](https://documentation.irs.uni-stuttgart.de/fsfw/development/api/task.html).

It also offers a unform API to execute periodic tasks in form of the
[`PeriodicTaskIF`](https://egit.irs.uni-stuttgart.de/fsfw/fsfw/src/branch/master/src/fsfw/tasks/PeriodicTaskIF.h).

These tasks can then be created using the
[`TaskFactory`](https://egit.irs.uni-stuttgart.de/fsfw/fsfw/src/branch/master/src/fsfw/tasks/TaskFactory.h)
singleton.

An arbitrary number of executable objects can then be passed to a periodic task. These objects
are then executed sequentially. This allows a granular design of executable tasks.
For example, important tasks get an own dedicated thread while other low priority objects are
scheduled consecutively in another thread.

In summary, task abstractions have the following advantages:

- Task execution can be deferred until an explicit `start` method is called
- Same uniform API across multiple operating systems

### Subtasks

 1. Load the required interfaces:
     - `#include "fsfw/tasks/ExecutableObjectIF.h"`
     - `#include "fsfw/tasks/PeriodicTaskIF.h`
     - `#include "fsfw/tasks/TaskFactory.h`
 2. For your three custom objects, implement the executable object IF provided by the framework
    instead of your custom interface.
 3. In your main function, create an instance of the `TaskFactory`. `TaskFactory`
    is implemented as a singleton: The object will create itself when using
    `TaskFactory::instance`. All subsequent calls will return the same intance.
    There are other similar singletons to create other objects like mutexes or message queues.
 3. Create two periodic tasks using the `TaskFactory::createPeriodicTask` function.
    Some notes on the expected arguments:
     - Each task has a name. This is useful for debugging, especially because the framework
       abstractons can detect missed deadlines.
     - The task priority parameter is OS dependent. This parameter is currently ignored for the
       Linux OSAL so you can pass 0 here.
       On Windows, you can retrieve the priority by using `tasks::makeWinPriority`
       which can be loaded by including `#include "fsfw/osal/windows/winTaskHelpers.h"`
     - The stack space parameter is generally ignored or unimportant for host systems.
       You can simply pass `PeriodicTaskIF::MINIMUM_STACK_SIZE` here. This parameter becomes
       important on resource constrained OSes and systems, for example FreeRTOS.
     - The frequency is expected as floating point seconds
     - You can pass a function which will be called when a deadline is missed. This is the case
       when a tasks took longer than its designated slot frequency. This is useful to detect
       bugs in the software or generally detect when tasks require a large amount of time.
       You can also pass `nullptr` here if you do not want any function to be called.
 4. Add the first two of your custom exec objects to the first periodic task. Those tasks
    will be executed in the same thread consecutively. You can use the `PeriodicTaskIF::addComponent`
    method to do this.
 5. Add the third custom exec object to the second periodic task. The third object
    gets an own thread
 6. Start both periodic tasks and add a permanent loop at the end of the main
    method which puts the main thread into sleep.

You successfully scheduled some objects using the framework!
The general concept of executble objects is used heavily throughout the framework.
For example, each device handler or controller is an executable object, as the bases
classes exposed by the framework implement `ExecutableObjectIF`.

There is also another type of periodic task handler called `FixedTimeslotTask`. Here,
you can explicitely specify (multiple) execution slots with a specified relative time within
the execution slot. This is useful for objects where there are multiple processing steps
but the steps take different amount of times.

In examples or other OBSW implementations using the framework, you will often
see the distinction between an `ObjectFactory.cpp` and an `InitMission.cpp`.
In the first file, all global (executable) objects will be created. In the second file,
all of these objects will be scheduled. Another chapter will introduce the Object Manager
to show what exactly is happening here.
