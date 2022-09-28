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

The goal of this task is to convert the code from task 1 so the [std::thread] API takes an
executable object to move to a more object oriented task approach. The printout of the thread
should remain the same. The executable objects should be named `MyExecutableObject`. It contains
one function called `periodicOperation` which performs the printout, and a static function which
takes the `MyExecutableObject` itself by reference and executes it in a permanent loop.

The executable object should be passed into the [std::thread] directly.

### Hints

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
  5. Add a constructor to `MyExecutableObject` which expects a millisecond delay
     as an `uint32_t` and cache it as a member variable. Then use this member
     variable in the `executeTask` implementation to make the task frequency configurable via the
     constructor (ctor) parameter.

With the conversion to executable object, we have reached a useful goal in object-oriented
programming (OOP) in general: The application logic inside `performOperation` is now decoupled
from the scheduling logic inside `executeTask`. This is also called seperation of concerns.

## 3. Making the executable objects generic

Our approach is useful buts lacks being generic as it relies on `std` library API. C++ as an OOP
language provides abstraction in form of interfaces, which can be used to have different types of
generic executable objects. Interfaces usually do not have a lot of source code on their own. They
describe a design contract a class should have which implements the interface. In general, the FSFW
relies heavily on subclassing and inheritance to provide adaptions point to users.

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
    have a ctor which expects a `MyExecutableObjectIF` by reference. It caches that object
    and exposes a `start` method to start the task

## 3. Using the framework abstractions

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

The task abstractions have the following advantages:

- Task execution can be deferred until an explicit `start` method is called
- Same uniform API across multiple operating systems

The goal of this task is to implement the task specified in 1 using the
abstractions provided in step 1.
