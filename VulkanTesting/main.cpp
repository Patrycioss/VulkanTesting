#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

int main() {
	std::cout << "Hello, World!" << std::endl;

	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}
	
	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello, World!", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
	}

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::cout << extensionCount << " extensions supported\n";

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	return 0;
}
