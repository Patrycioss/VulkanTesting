﻿#pragma once

#define GLFW_INCLUDE_VULKAN
#include <string>
#include <vector>

#include "glfw/glfw3.h"

class HelloTriangleApp {
public: // Properties

private: // Member Variables
	const uint32_t WINDOW_WIDTH = 800;
	const uint32_t WINDOW_HEIGHT = 600;

	const VkDebugUtilsMessageTypeFlagsEXT MESSAGE_TYPES =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	const VkDebugUtilsMessageSeverityFlagsEXT MESSAGE_SEVERITY =
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		

	const std::vector<const char*> VALIDATION_LAYERS = {
		"VK_LAYER_KHRONOS_validation"
	};

	const std::vector<const char*> ADDITIONAL_REQUIRED_EXTENSIONS = {
		VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, // Mac stuff?
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME, // Debug Message Callback
	};

#ifdef NDEBUG
	const bool ENABLE_VALIDATION_LAYERS = false;
#else
	const bool ENABLE_VALIDATION_LAYERS = true;
#endif

	GLFWwindow* windowHandle{};
	VkInstance instance{};
	VkDebugUtilsMessengerEXT debugMessenger{};

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

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo) const;
	void createDebugMessenger();

	void createWindow();
};
