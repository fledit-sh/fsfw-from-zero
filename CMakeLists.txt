cmake_minimum_required(VERSION 3.22.0)
# We can version our project using CMake
project(fsfw-from-zero VERSION 0.1.0)

# Sometimes, a C++ project might require a certain C++ standard to build.
# The following directives make sure that the compiler supports the required
# standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the framework dependency
set(FSFW_OSAL host CACHE STRING "FSFW OSAL")
set(FSFW_CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(fsfw)

# Add our executable and its only source file
add_executable(fsfw-from-zero main.cpp)

# Link the framework so we can use it from our application
target_link_libraries(fsfw-from-zero PRIVATE fsfw)
