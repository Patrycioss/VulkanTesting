cmake_minimum_required(VERSION 3.30)
project(HelloTriangleApp)
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES *.cpp *.hpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../deps deps)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../utils utils)
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} deps)