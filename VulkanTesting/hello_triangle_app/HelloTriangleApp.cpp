#include "HelloTriangleApp.hpp"

#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <string.h>
#include <vector>
#include <bits/stl_algo.h>

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
	createSwapChain();
}

HelloTriangleApp::~HelloTriangleApp() {
	vkDestroySwapchainKHR(device, swapChain, nullptr);
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

HelloTriangleApp::SwapChainSupportDetails HelloTriangleApp::querySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
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

	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);

	if (!deviceFeatures.geometryShader ||
		!findQueueFamilies(device).isComplete() ||
		!checkDeviceExtensionSupport(device) ||
		!swapChainSupport.isValid()) {
		return 0;
	}

	return score;
}

VkSurfaceFormatKHR HelloTriangleApp::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	// For now just take the first one, otherwise rank them based on how good they are.
	return availableFormats[0];
}

VkPresentModeKHR HelloTriangleApp::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D HelloTriangleApp::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}

	int32_t width;
	int32_t height;

	glfwGetFramebufferSize(windowHandle, &width, &height);

	VkExtent2D actualExtent = {
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height),
	};

	actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

	return actualExtent;
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

void HelloTriangleApp::createSwapChain() {
	SwapChainSupportDetails swapChainSupportDetails = querySwapChainSupport(physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupportDetails.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupportDetails.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupportDetails.capabilities);

	uint32_t imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;

	if (swapChainSupportDetails.capabilities.maxImageCount > 0 && imageCount > swapChainSupportDetails.capabilities.maxImageCount) {
		imageCount = swapChainSupportDetails.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.presentMode = presentMode;

	// Ignores color of pixels that are not visible.
	createInfo.clipped = VK_TRUE;

	// For if old swap chain becomes invalid and a new one needs to be created. During resize for example.
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	// Applies certain transformations (like rotations) to all images on the chain. CurrentTransform for normal.
	createInfo.preTransform = swapChainSupportDetails.capabilities.currentTransform;

	// For blending with other windows.
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	//The imageUsage bit field specifies what kind of operations we’ll use the images in the swap chain for.
	//We’re going to render directly to them, which means that they’re used as color attachment.
	//It is also possible that you’ll render images to a separate image first to perform operations like post-processing.
	//In that case you may use a value like VK_IMAGE_USAGE_TRANSFER_DST_BIT instead and use a memory operation to transfer the rendered image to a swap chain image.
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	const uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	// Sharing mode concurrent for now if they are the same as ownership is more difficult.
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	const VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
	if (result != VK_SUCCESS) {
		UTIL_THROW("Failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}
