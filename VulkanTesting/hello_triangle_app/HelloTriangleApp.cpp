#include "HelloTriangleApp.hpp"

#include <iostream>
#include <map>
#include <set>
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

	createWindow();
	createVKInstance();
	createDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
}

HelloTriangleApp::~HelloTriangleApp() {
	vkDestroyDevice(device, nullptr);

	if (ENABLE_VALIDATION_LAYERS) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(windowHandle);
	glfwTerminate();
}

void HelloTriangleApp::Run() {
	while (!glfwWindowShouldClose(windowHandle)) {
		glfwPollEvents();
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

std::vector<const char*> HelloTriangleApp::getRequiredExtensions() const {
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

void HelloTriangleApp::createSurface() {
	const VkResult result = glfwCreateWindowSurface(instance, windowHandle, nullptr, &surface);
	if (result != VK_SUCCESS) {
		UTIL_THROW("Failed to create window surface!");
	}
}

bool HelloTriangleApp::checkDeviceExtensionSupport(const VkPhysicalDevice device) const {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

int32_t HelloTriangleApp::rateDeviceSuitability(const VkPhysicalDevice device) {
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	int32_t score = 0;

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	score += deviceProperties.limits.maxImageDimension2D;

	if (!deviceFeatures.geometryShader ||
		!findQueueFamilies(device).isComplete() ||
		!checkDeviceExtensionSupport(device)) {
		return 0;
	}

	return score;
}

void HelloTriangleApp::pickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		UTIL_THROW("Failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	std::multimap<int32_t, VkPhysicalDevice> candidates;

	for (const auto& device : devices) {
		int score = rateDeviceSuitability(device);
		if (score > 0) {
			candidates.insert(std::pair(score, device));
		}
	}

	if (candidates.size() > 0) {
		physicalDevice = candidates.begin()->second;
	} else
		UTIL_THROW("Failed to find a suitable GPU!");
}

HelloTriangleApp::QueueFamilyIndices HelloTriangleApp::findQueueFamilies(const VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int32_t i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

void HelloTriangleApp::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};

	queueCreateInfos.reserve(uniqueQueueFamilies.size());

	float queuePriority = 1.0f;

	for (const auto& queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.emplace_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());
	createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

	if (ENABLE_VALIDATION_LAYERS) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
		createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	const VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
	if (result != VK_SUCCESS) {
		UTIL_THROW("Failed to create logical device!");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}
