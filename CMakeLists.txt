cmake_minimum_required(VERSION 3.1)
project(Project_filter C)

find_package(MPI REQUIRED)

set(CMAKE_C_COMPILER mpicc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")

add_library(PPMtypes PPMtypes.c)

add_executable(filter filter.c)
target_include_directories(filter PRIVATE ${MPI_INCLUDE_PATH})
target_link_libraries(filter PRIVATE ${MPI_C_LIBRARIES} PPMtypes)