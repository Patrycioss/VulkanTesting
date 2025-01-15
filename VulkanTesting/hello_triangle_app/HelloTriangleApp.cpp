#include "HelloTriangleApp.hpp"

#include <iostream>
#include <string.h>
#include <vector>

#include "../utils/log.hpp"

HelloTriangleApp::HelloTriangleApp() {
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

	CreateVKInstance();
}

HelloTriangleApp::~HelloTriangleApp() {
	glfwDestroyWindow(windowHandle);
	glfwTerminate();
}

void HelloTriangleApp::Run() {
	CreateWindow();

	while (!glfwWindowShouldClose(windowHandle)) {
		glfwPollEvents();
	}
}

void HelloTriangleApp::CreateVKInstance() {
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

	uint32_t requiredExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionCount);

	std::vector<const char*> requiredExtensions;
	requiredExtensions.reserve(requiredExtensionCount);

	for (uint32_t i = 0; i < requiredExtensionCount; i++) {
		requiredExtensions.emplace_back(glfwExtensions[i]);
	}

	requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

	uint32_t availableExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

	std::vector<const char*> extensions;
	extensions.reserve(availableExtensionCount);

	for (int i = 0; i < availableExtensions.size(); i++) {
		for (int j = requiredExtensions.size() - 1; j >= 0; j--) {
			if (strcmp(requiredExtensions[j], availableExtensions[i].extensionName) == 0) {
				requiredExtensions.erase(requiredExtensions.begin() + j);
			}
		}
		extensions.push_back(availableExtensions[i].extensionName);
	}

	if (requiredExtensions.size() > 0) {
		std::string unavailableExtensions = "";
		for (const auto& extension : requiredExtensions) {
			unavailableExtensions += extension;
			unavailableExtensions += " ";
		}
	}

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(availableExtensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount = 0;
	createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		UTIL_THROW("Failed to create Vulkan instance!");
	}
}

void HelloTriangleApp::CreateWindow() {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	windowHandle = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello, World!", nullptr, nullptr);
	if (!windowHandle) {
		throw std::runtime_error("Failed to create window!");
	}

	glfwMakeContextCurrent(windowHandle);
	glfwSwapInterval(1);
}
