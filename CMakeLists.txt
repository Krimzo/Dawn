cmake_minimum_required(VERSION 4.4.0)
project(Dawn LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE dawn_sources CONFIGURE_DEPENDS source/*.cpp)
add_library(dawnlib ${dawn_sources})
target_include_directories(dawnlib PUBLIC source)

add_executable(dawn main.cpp)
target_link_libraries(dawn PUBLIC dawnlib)
set_target_properties(dawn PROPERTIES
    DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
