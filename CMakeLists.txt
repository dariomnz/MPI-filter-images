cmake_minimum_required(VERSION 3.1)
project(Project_filter C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

add_library(PPMtypes PPMtypes.c)

add_executable(filter filter.c)
target_link_libraries(filter PRIVATE ${MPI_C_LIBRARIES} PPMtypes pthread)