# Global Addressable Objects and the Object Manager

The FSFW is an object-oriented framework and uses the concept of classes and objects to model a
remote system like a satellite. Usually, every non-trivial object in the flight software is assigned
a 32-bit object ID. This ID is then used as an address field for that object. Lets say for example
that you want to change the ACS controller mode and the controller has the object ID 0x12345678.
You would then send a mode command to the object 0x12345678 to do this task. In general, using
objects allows software developers to model the architecture of the satellite and also makes it
easier for SW developers to reason with Operations about what the satellite should be capable of.

Some other examples of addressable building blocks of a software built with the FSFW could be

 - Device handlers for external sensors or payloads
 - Assembly components which manage device redundancy
 - Subsystem components which perform the mode and health management of related device, assembly
   and controller objects

The framework also has a global singleton class to store global objects and retrieve them back
in an arbitrary format (e.g. only a certain interface of an object) at a later point.

The required interface of a class to be compatible to the object manager is the `SystemObjectIF`.
The `SystemObject` class is a base class implementing this interface which is implemented
by most base classes in the framework.

It is recommended to do the task workshop located inside `ws-tasks` before doing this workshop,
unless you are familiar with how task scheduling with the framework works.

# 1. Creating a user `SystemObject`

In this chapter, a custom class will be created which is insertable into the global object manager.

## Subtasks

 1. Create a custom class `MySystemObject` which implements the
    [`SystemObject`](https://documentation.irs.uni-stuttgart.de/fsfw/development/api/objectmanager.html#systemobject) 
    base class. Use the object ID 0x10101010. The second argument of the
    `SystemObject` constructor can be used to disable object manager registration.
    Use it to do exactly that.
 2. Override the `initialize` function and print out a test string in the function.
 3. Create a dynamic instance of that class on the heap using the
    [`new`](https://en.cppreference.com/w/cpp/language/new) keyword.
 4. Print out the object ID in hex format with 8 digits. You can use the `iostream`
    manipulators [setw](https://en.cppreference.com/w/cpp/io/manip/setw),
    [setfill](https://en.cppreference.com/w/cpp/io/manip/setfill) and
    [hex](https://en.cppreference.com/w/cpp/io/manip/hex) to do this. You need
    to include the `iomanip` C++ system header t ouse those.
 5. Call the `initialize` function of your dynamic object
 6. Explicitely [delete](https://en.cppreference.com/w/cpp/keyword/delete) your global object.
    Forgetting to delete dynamic resources in C++ is generally a resource leak because the memory
    claimed for creating that dynamic resource can not be re-used by the OS.   

## Hints

 - You can use `#include "fsfw/objectmanager.h"` to include everything you need.
 - The `SystemObject` base class receives its object ID information by constructor argument.
   Every base (parent) class which does not have a default (empty) constructor needs to be
   initialized by the child class constructor. You can do this in the child class
   [constructor member initializer list](https://en.cppreference.com/w/cpp/language/constructor)

## Notes on memory and resource management

In desktop programs, it is very common to simply dynamically allocate all required resources
as they are required. It should be noted that dynamic memory allocation can show non-deterministic
behaviour, which is something that should be avoided in real-time environments. Especially on
smaller systems, where the RAM might be limited to something like for example 1 MB, one has to be
really careful with dynamic memory management to not run out of memory during run-time.

A possible side-effect
of running out of memory would be that the allocation can take a possibly infinite time. Another
side-effect which is probably more common is that the allocation simply fails and a `nullptr` is
returned, which causes the application to crash unless every allocation call is checked.

Omitting dynamic memory allocation altogether is not really a acceptable solution either unless
dealing with really, REALLY (!) small systems like a PIC microcontroller. A good solution is
to limit the dynamic memory allocation to the program initialization time and only use pre-allocated
memory during run-time. This is what the FSFW or real time OSes like RTEMS generally promote and
support.

It is also important to keep in mind that `std` library containers generally allocate dynamically
when inserting new entries.

# 2. Initialize the object using the `ObjectManager`

The `SystemObject` base class will take care of automatically registering the object at the
global object manager as part of its constructor. The object manager stores all inserted objects
by the `SystemObjectIF` base class pointer inside a hash map, so all inserted objects can be
retrieved at a later stage. The object manager is also able to call the `initialize` method of
all its registered objects. The initialize method allows to return an explicit returnvalue
for failed object initialization. This is generally not possible for object constructors.
The usual way to have an object construction fail is to use exceptions, which might or might not
be available to your project.

## Subtasks

 1. Register the `MySystemObject` class into the global object manager. You can do this with a
    simple tweak of the base class constructor.
 2. Remove the `delete` call. The object manager will delete all of its contained objects
    automatically in its own destructor
 3. Retrieve the global instance of the object manager using its static `instance` method
    and use it to initialize all system objects including your custom system object.
 4. Retrieve the concrete instance of your object using the `ObjectManager` `get` method.
    Please note that you explicitely have to specify the target type you want to retrieve
    using a template argument to `get`. Use that instance to retrieve and print the object ID
    instead of using the instance returned by `new`

# 3. Schedule your object using its object ID

The object ID is now an addressing unit which can be used at various places in the framework.
One example is to schedule the object. This means that instead of passing the concrete instance
of the object, you can also add units to schedule by using their object ID.

## Subtasks

 1. Retrieve the global instance of the `TaskFactory` using its static `instance` method.
 2. Create a new enum called `ObjectIds` and make your object ID constant an enum number
    if it. If this is not the case already the case, refactor your `MySystemObject` to expect
    the Object ID via constructor argument and pass your enum member as the object ID.
 3. Add the `ExecutableObjectIF` to the list of implemented interface in `MySystemObject`
    and rename it to `MyObject` to make it executable. Most IDEs have some functionality
    to make renaming an object as convenient as possible.
 3. Create a `PeriodicTask` and add your custom system object using its object ID with the
    `addComponent` method.
 4. Schedule the object. Do not forget to put the main thread to sleep, for example by using
    code like this

    ```cpp
    while(true) {
       using namespace std::chrono_literals;
       this_thread::sleep_for(5000ms);
    }
    ```

## Hints

- You can use `#include "fsfw/tasks/TaskFactory.h"` to include everything you need.

## General note on global mutable objects

Please note that the object manager is a software entity which global mutable state. This
is something which can easily introduce subtle and dangerous bugs into a multi-threaded
software. If you are sharing an object with the manager between multiple threads, all object
access needs to be protected explicitely with concurrency tools like a Mutex by the developer.

The object manager has no own capabilities to ensure thread-safey in such a case.
It is recommended to do the `ws-ipc` workshop to get familiar with various ways for objects
to communicate with each other in a thread-safe way.
