cmake_minimum_required(VERSION 3.10)

project(raylib-test)

# set(CMAKE_C_STANDARD 11)

set(RAYLIB_DIR ${CMAKE_SOURCE_DIR}/raylib-5.0_linux_amd64)

include_directories(${RAYLIB_DIR}/include)
find_library(RAYLIB_LIBRARY raylib HINTS ${RAYLIB_DIR}/lib)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIBRARY} m GL pthread dl rt X11)
