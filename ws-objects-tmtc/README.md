# Object Manager

The FSFW is an object-oriented framework and uses the concept of classes and objects to model the
satellite. Usually, every non-trivial object in the flight software is assigned a 32-bit object ID.
This ID is then used as an address field for various command types.

The framework also has a global singleton class to store global objects and retrieve them back
in an arbitrary format (e.g. only a certain interface of an object) at a later point.

The required interface of a class to be compatible to the object manager is the `SystemObjectIF`.
The `SystemObject` class is a base class implementing this interface which is implemented
by most base classes in the framework.

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

 - The `SystemObject` base class receives its object ID information by constructor argument.
   Every base (parent) class which does not have a default (empty) constructor needs to be
   initialized by the child class constructor. You can do this in the child class
   [constructor member initializer list](https://en.cppreference.com/w/cpp/language/constructor)

## Notes on memory and resource management

In desktop programs, it is very common to simply dynamically allocate all required resources
as they are required. It should be noted that dynamic memory allocation can show non-deterministic
behaviour, which is non-favorable in real-time environments. Especially on smaller systems, where
the RAM might be limited to something like for example 1 MB, one has to be really careful with
dynamic memory management to not run out of memory during run-time. A possible side-effect
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
by the `SystemObject` base class pointer inside a hash map, so all inserted objects can be
retrieved at a later stage. The object manager is also able to call the `initialize` method of
all its registered objects.

## Subtasks

