add_executable(fsfw-from-zero)
target_sources(fsfw-from-zero PRIVATE main.cpp)
target_include_directories(fsfw-from-zero PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
