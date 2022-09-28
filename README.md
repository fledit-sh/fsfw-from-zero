FSFW From Zero Workshop
=======

This workshop gives an introduction to the Flight Software Framework,
starting from a simple hello world program in C++. As such, it it also suitable
for C++ beginners.

Start by cloning this repository and updating the submodules to also clone
the [Flight Software Framework](https://egit.irs.uni-stuttgart.de/fsfw):

```sh
git clone https://egit.irs.uni-stuttgart.de/fsfw/fsfw-from-zero.git
git submodule init
git submodule update
```

# Overview

This workshop does an incremental build-up of a simple software which
is similar to an On-Board Software. It is organised in chapters which have multiple
tasks. For each task, a solution source file will be provided. in a related subfolder with the
same name.

It is recommended to have a basic understanding of C++ basics and object-oriented programming
in general before doing this workshop. There are various books and online resources available to
learn this.

## Tasks Workshop

This chapter provides an introduction into the thread/tasks abstractions provided by the framework. The chapter descriptions and solutions are
locaited inside `01-tasks`.

It is recommended to start with this workshop.
