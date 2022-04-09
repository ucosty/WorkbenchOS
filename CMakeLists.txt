cmake_minimum_required(VERSION 3.21)
project(WorkbenchOS)

set(CMAKE_CXX_STANDARD 23)
enable_language(CXX ASM)

include_directories(Library)
include_directories(Libraries)

add_subdirectory(Libraries)
add_subdirectory(Bootloader)
add_subdirectory(Kernel)
