cmake_minimum_required(VERSION 3.30)
project(HelloTriangleApp)
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES *.cpp *.hpp)
file(COPY resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../deps deps)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../utils utils)
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(${PROJECT_NAME} deps)
