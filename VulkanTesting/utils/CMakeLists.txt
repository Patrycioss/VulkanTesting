cmake_minimum_required(VERSION 3.30)
project(utils)
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES *.cpp *.hpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)