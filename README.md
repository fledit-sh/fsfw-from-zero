FSFW From Zero Workshop
=======

This workshop gives an introduction to the Flight Software Framework,
starting from a simple hello world program in C++. As such, it it also suitable
for people with basic C++ skills who want to learn how the FSFW works and how to build
On-Board Software (OBSW) in general. This workshop does not rely on external hardware
and can be done on a host machine (e.g. laptop).

# Requirements

I really recommend doing this in an Unix environment. If you do not have
or do not want to use a VM, consider using [Ubuntu WSL](https://ubuntu.com/wsl) on Windows.

1. [`git`](https://git-scm.com/) installed
2. [`cmake`](https://cmake.org/) installed as the build system generator
3. C++ compiler installed. On Windows, you can use [MinGW64](https://www.msys2.org/) or
   [clang](https://releases.llvm.org/download.html). On Unix, you can use the pre-installed GCC
4. Build system for C/C++ installed. For Windows, it is recommended to use [ninja](https://ninja-build.org/).
   On Unix, you can use the pre-installed `make` tool.

# Getting started

Start by cloning this repository and updating the submodules to also clone
the [Flight Software Framework](https://egit.irs.uni-stuttgart.de/fsfw):

```sh
git clone https://egit.irs.uni-stuttgart.de/fsfw/fsfw-from-zero.git
git submodule init
git submodule update
```

This workshop uses CMake like the framework to build the application.
This command sequence will build the software

```sh
mkdir build
cd build && cmake ..
cmake --build . -j
```

This will generate the `fsfw-from-zero` executable inside the build folder.
It is recommended to use an IDE like VS Code or CLion. Those generally have good
CMake support.

# Overview

This workshop is organised in chapters which have multiple tasks. For each task, solution source
files will be provided but you are encouraged to work to the solution on your own.

It is recommended to have a basic understanding of C++ basics and object-oriented programming
in general before doing this workshop. There are various books and online resources available to
learn this.

In general, it is recommended to start with the `ws-tasks` workshop and then move to the
`ws-objects` workshop.

## Tasks workshop

This chapter provides an introduction into the thread/tasks abstractions provided by the framework.
The chapter descriptions and solutions are located inside `ws-tasks`.

## Objects and Object Manager workshop

This chapter will introduce the object manager and expand the knowledge of the tasks workshop
by showing how to conveniently create global addressable objects. The chapter description and
solutions are located inside `ws-objects`.

## WIP: TMTC workshop

Provides an introduction into TMTC handling, as virtually all space systems are remote systems
where telemetry and telecommands are the primary data interface available to communicate with the
satellite.

## WIP: Structuring your project and managing third-party dependencies with CMake

OBSW usually becomes very complex as more and more features and functionality is added.
We need to split the software into smaller dedicated modules as this happens to keep complexity
in check. This workshop shows how this is done with CMake. Another common
task is the integration of third-party libraries. In general, this is a painful process
in C/C++ with no built-in package management and an archaic header system. This workshop
will attempt to alleviate some of the pain by showing how to integrate a CMake compatible library
by example and how to intgerate a library without CMake support.


## WIP: Controller workshop

This chapter will introduce the `ControllerBase` and `ExtendedControllerBase` class
and the various helper interfaces they expose.
