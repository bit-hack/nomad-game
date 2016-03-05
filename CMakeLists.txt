cmake_minimum_required(VERSION 3.3)
project(nomad)

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})

add_subdirectory(nomad-headers)
add_subdirectory(nomad-window)
add_subdirectory(nomad-util)
add_subdirectory(nomad-net)
add_subdirectory(nomad-server)
add_subdirectory(nomad-game)
add_subdirectory(nomad-human)
add_subdirectory(nomad-ai)
add_subdirectory(nomad)
