cmake_minimum_required(VERSION 3.10)
project(ConwaysGameOfLife C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(SDL2 REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -march=native -fopenmp")

set(SOURCES
    src/main.c
    src/config.c
    src/core.c
    src/ui.c
)

add_executable(game ${SOURCES})

target_include_directories(game PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(game PRIVATE SDL2main SDL2 m)
