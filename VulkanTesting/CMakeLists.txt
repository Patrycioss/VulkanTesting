cmake_minimum_required(VERSION 3.30)
include(FetchContent)
project(VulkanTesting)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(hello_triangle_app)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} HelloTriangleApp)