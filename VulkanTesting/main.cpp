#define GLFW_INCLUDE_VULKAN

#include <exception>
#include <iostream>
#include <ostream>

#include "hello_triangle_app/HelloTriangleApp.hpp"

int main() {
	try {
		HelloTriangleApp app;
		app.Run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}
