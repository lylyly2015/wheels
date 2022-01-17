cmake_minimum_required(VERSION 3.0)

project(THREADPROJECT)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_BUILD_TYPE Debug)

#add_subdirectory(m_thread_pool)

add_executable(main_cmake memorypool/main.cc)

#target_link_libraries(main_cmake m_thread_pool)

#include_directories(m_thread_pool.h task.h)