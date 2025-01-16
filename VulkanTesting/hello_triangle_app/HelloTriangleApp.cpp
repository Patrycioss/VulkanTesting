#include "HelloTriangleApp.hpp"

#include <iostream>
#include <string.h>
#include <vector>

#include "../utils/log.hpp"

HelloTriangleApp::HelloTriangleApp() {
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

	createVKInstance();
}

HelloTriangleApp::~HelloTriangleApp() {
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(windowHandle);
	glfwTerminate();
}

void HelloTriangleApp::Run() {
	createWindow();

	while (!glfwWindowShouldClose(windowHandle)) {
		glfwPollEvents();
	}
}

std::vector<const char*> HelloTriangleApp::getRequiredExtensions() {
	uint32_t requiredExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionCount);

	std::vector<const char*> requiredExtensions;

	// requiredExtensionCount += 1;
	requiredExtensions.reserve(requiredExtensionCount);

	for (uint32_t i = 0; i < requiredExtensionCount; i++) {
		requiredExtensions.emplace_back(glfwExtensions[i]);
	}

	requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	return requiredExtensions;
}

void HelloTriangleApp::checkValidationLayerSupport() const {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	std::string unavailableLayers = "";
	bool layersAvailable = true;

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			layersAvailable = false;
			unavailableLayers += layerName;
			unavailableLayers += " ";
		}
	}

	if (!layersAvailable) {
		UTIL_THROW("Requested validation layers not supported " + unavailableLayers);
	}
}

VkApplicationInfo HelloTriangleApp::createApplicationInfo() const {
	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "Hello Triangle";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "No Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	return applicationInfo;
}

void HelloTriangleApp::createVKInstance() {
	if (enableValidationLayers) {
		checkValidationLayerSupport();
	}

	const VkApplicationInfo applicationInfo = createApplicationInfo();

	std::vector<const char*> requiredExtensions = getRequiredExtensions();

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
		extensions.emplace_back(availableExtensions[i].extensionName);
	}

	if (requiredExtensions.size() > 0) {
		std::string unavailableExtensions = "";
		for (const auto& extension : requiredExtensions) {
			unavailableExtensions += extension;
			unavailableExtensions += " ";
		}

		UTIL_THROW("Requested validation extensions not supported: " + unavailableExtensions);
	}

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	const auto vkCreateResult = vkCreateInstance(&createInfo, nullptr, &instance);

	if (vkCreateResult != VK_SUCCESS) {
		UTIL_THROW("Failed to create Vulkan instance!");
	}
}

void HelloTriangleApp::createWindow() {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	windowHandle = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello, World!", nullptr, nullptr);
	if (!windowHandle) {
		throw std::runtime_error("Failed to create window!");
	}

	glfwMakeContextCurrent(windowHandle);
	glfwSwapInterval(1);
}
