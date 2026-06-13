cmake_minimum_required(VERSION 3.16)
project(LibraryManagementSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(lms main.cpp)

if(MSVC)
    target_compile_options(lms PRIVATE /W4 /permissive-)
else()
    target_compile_options(lms PRIVATE -Wall -Wextra -Wpedantic)
endif()
