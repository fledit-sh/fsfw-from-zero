cmake_minimum_required(VERSION 3.22.0)
# We can version our project using CMake
project(foo VERSION 0.1.0)

# Sometimes, a C++ project might require a certain C++ standard to build.
# The following directives make sure that the compiler supports the required
# standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(BUILD_FINISHED)
  add_subdirectory(finished)
endif()
add_subdirectory(start)

set(FSFW_OSAL host CACHE STRING "FSFW OSAL")
set(FSFW_CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(fsfw)
target_link_libraries(fsfw-from-zero PRIVATE fsfw)
if(BUILD_FINISHED)
  target_link_libraries(fsfw-from-zero-done PRIVATE fsfw)
endif()
