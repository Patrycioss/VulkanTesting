#pragma once

#define GLFW_INCLUDE_VULKAN
#include "glfw/glfw3.h"

class HelloTriangleApp {
public: // Properties

private: // Member Variables
	const uint32_t WINDOW_WIDTH = 800;
	const uint32_t WINDOW_HEIGHT = 600;
	
	GLFWwindow* windowHandle{};
	VkInstance instance{};

public: // Public Functions
	HelloTriangleApp();
	~HelloTriangleApp();
	
	HelloTriangleApp(const HelloTriangleApp&) = delete;
	HelloTriangleApp(HelloTriangleApp&&) = delete;
	HelloTriangleApp& operator=(const HelloTriangleApp&) = delete;

	void Run();

private: // Private Methods
	void CreateVKInstance();
	void CreateWindow();
};
