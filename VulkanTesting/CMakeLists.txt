cmake_minimum_required(VERSION 3.30)
include(FetchContent)
project(VulkanTesting)

find_package(Vulkan REQUIRED)
set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)

FetchContent_MakeAvailable(glfw glm)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} glfw glm Vulkan::Vulkan)