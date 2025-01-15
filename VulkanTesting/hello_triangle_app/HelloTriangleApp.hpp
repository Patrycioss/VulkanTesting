#pragma once

#define GLFW_INCLUDE_VULKAN
#include "glfw/glfw3.h"

class HelloTriangleApp {
public: // Properties

private: // Member Variables
	GLFWwindow* windowHandle{};

public: // Public Functions
	HelloTriangleApp();
	~HelloTriangleApp();
	
	HelloTriangleApp(const HelloTriangleApp&) = delete;
	HelloTriangleApp(HelloTriangleApp&&) = delete;
	HelloTriangleApp& operator=(const HelloTriangleApp&) = delete;

	void Run();

private: // Private Methods
	void Initialize();
	void CreateWindow();
};
