#pragma once

#define GLFW_INCLUDE_VULKAN
#include <string>
#include <vector>

#include "glfw/glfw3.h"

class HelloTriangleApp {
public: // Properties

private: // Member Variables
	const uint32_t WINDOW_WIDTH = 800;
	const uint32_t WINDOW_HEIGHT = 600;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

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
	std::vector<const char*> getRequiredExtensions();
	void checkValidationLayerSupport() const;

	VkApplicationInfo createApplicationInfo() const;

	void createVKInstance();
	void createWindow();
};
