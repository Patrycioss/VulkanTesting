#include "HelloTriangleApp.hpp"

#include <iostream>
#include <string.h>
#include <vector>

#include "../utils/log.hpp"

static VkResult CreateDebugUtilsMessengerEXT(const VkInstance instance,
                                             const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                             const VkAllocationCallbacks* pAllocator,
                                             VkDebugUtilsMessengerEXT* pDebugMessenger) {
	const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void DestroyDebugUtilsMessengerEXT(const VkInstance instance,
                                          const VkDebugUtilsMessengerEXT debugMessenger,
                                          const VkAllocationCallbacks* pAllocator) {
	auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

static unsigned DebugCallback(
	const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	const VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	std::string type = "";

	switch (messageType) {
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
			type = "General";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
			type = "Validation";
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
			type = "Performance";
			break;
		default:
			type = "Unknown";
			break;
	}

	switch (messageSeverity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			UTIL_WARN(type + " Vulkan Warning: " + pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			UTIL_ERR(type + " Vulkan Error: " + pCallbackData->pMessage);
			break;
		default:
			UTIL_LOG(type + " Vulkan Log: " + pCallbackData->pMessage);
			break;
	}

	return VK_FALSE;
}

HelloTriangleApp::HelloTriangleApp() {
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

	createVKInstance();
}

HelloTriangleApp::~HelloTriangleApp() {
	if (ENABLE_VALIDATION_LAYERS) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

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

	requiredExtensions.reserve(requiredExtensionCount + ADDITIONAL_REQUIRED_EXTENSIONS.size());

	for (uint32_t i = 0; i < requiredExtensionCount; i++) {
		requiredExtensions.emplace_back(glfwExtensions[i]);
	}

	for (uint32_t i = 0; i < ADDITIONAL_REQUIRED_EXTENSIONS.size(); i++) {
		requiredExtensions.emplace_back(ADDITIONAL_REQUIRED_EXTENSIONS[i]);
	}

	return requiredExtensions;
}

void HelloTriangleApp::checkValidationLayerSupport() const {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	std::string unavailableLayers = "";
	bool layersAvailable = true;

	for (const char* layerName : VALIDATION_LAYERS) {
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
	if (ENABLE_VALIDATION_LAYERS) {
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

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (ENABLE_VALIDATION_LAYERS) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
		createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

		populateDebugMessengerCreateInfo(&debugCreateInfo);
		createInfo.pNext = &debugCreateInfo;
	} else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	const auto vkCreateResult = vkCreateInstance(&createInfo, nullptr, &instance);

	if (vkCreateResult != VK_SUCCESS) {
		UTIL_THROW("Failed to create Vulkan instance!");
	}
}

void HelloTriangleApp::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo) const {
	createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo->messageSeverity = MESSAGE_SEVERITY;
	createInfo->messageType = MESSAGE_TYPES;
	createInfo->pfnUserCallback = DebugCallback;
	createInfo->pUserData = nullptr;
}

void HelloTriangleApp::createDebugMessenger() {
	if (!ENABLE_VALIDATION_LAYERS) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(&createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		UTIL_THROW("Failed to create debug messenger!");
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
