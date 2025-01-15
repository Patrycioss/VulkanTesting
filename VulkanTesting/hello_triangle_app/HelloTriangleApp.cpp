#include "HelloTriangleApp.hpp"

#include <iostream>

HelloTriangleApp::HelloTriangleApp() {
}

HelloTriangleApp::~HelloTriangleApp() {
	glfwDestroyWindow(windowHandle);
	glfwTerminate();
}

void HelloTriangleApp::Run() {
	Initialize();
	CreateWindow();

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::cout << extensionCount << " extensions supported\n";

	while (!glfwWindowShouldClose(windowHandle)) {
		glfwPollEvents();
	}
}

void HelloTriangleApp::Initialize() {
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}
}

void HelloTriangleApp::CreateWindow() {
	windowHandle = glfwCreateWindow(640, 480, "Hello, World!", nullptr, nullptr);
	if (!windowHandle) {
		throw std::runtime_error("Failed to create window!");
	}

	glfwMakeContextCurrent(windowHandle);
	glfwSwapInterval(1);
}
