cmake_minimum_required(VERSION 3.16.0)
# We can version our project using CMake
project(fsfw-from-zero VERSION 0.1.0)

# Sometimes, a C++ project might require a certain C++ standard to build.
# The following directives make sure that the compiler supports the required
# standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the framework dependency. If the FSFW submodule is not present we still
# want to be able to compile the basic workshop executables. In that case we
# provide a dummy interface target so the rest of the CMake configuration keeps
# working without requiring the actual framework checkout.
set(FSFW_OSAL host CACHE STRING "FSFW OSAL")
set(FSFW_CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(FSFW_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/fsfw)
if(EXISTS ${FSFW_SOURCE_DIR}/CMakeLists.txt)
    add_subdirectory(${FSFW_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/fsfw)
else()
    message(WARNING "FSFW submodule not found. Building with stub library.")
    add_library(fsfw INTERFACE)
endif()

# Add our executable and its only source file
add_executable(fsfw-from-zero main.cpp)

# Link the framework so we can use it from our application
target_link_libraries(fsfw-from-zero PRIVATE fsfw)
