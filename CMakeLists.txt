cmake_minimum_required(VERSION 3.22.0)
# We can version our project using CMake
project(foo VERSION 0.1.0)

# Sometimes, a C++ project might require a certain C++ standard to build.
# The following directives make sure that the compiler supports the required
# standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This adds a library target called "foo"
add_executable(fsfw-from-zero)
target_sources(fsfw-from-zero PRIVATE main.cpp)

# target_link_libraries(fsfw-from-zero PRIVATE fsfw)
