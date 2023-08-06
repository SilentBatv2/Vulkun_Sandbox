
#include"Application.hpp"


int my_clamp(unsigned int val, unsigned int min, unsigned int max)
{
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}

Application::Application()
{
	window = nullptr;
	instance = VK_NULL_HANDLE;
	physicalDevice = VK_NULL_HANDLE;
	surface = VK_NULL_HANDLE;
	swapChain = VK_NULL_HANDLE;
}

void Application::InitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan_Sandbox", nullptr, nullptr);
}
void Application::InitVulkan()
{
	CreateInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createSwapChain();
}

void Application::CreateInstance()
{
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0;
	std::vector<const char*> glfwExtensions(getRequiredExtensions(&glfwExtensionCount));

#ifdef _DEBUG

	//Checking presence of instance extensions

	unsigned int extentioncount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extentioncount, nullptr);

	VkExtensionProperties* extensions = new VkExtensionProperties[extentioncount];

	vkEnumerateInstanceExtensionProperties(nullptr, &extentioncount, extensions);

	std::cout << "available extensions:\n";

	for (unsigned int i = 0; i < extentioncount; ++i) {
		std::cout << '\t' << extensions[i].extensionName << '\n';
	}

	delete[] extensions;


	std::cout << glfwExtensionCount << '\n';
	for (unsigned int i = 0; i < glfwExtensionCount; ++i)
	{
		std::cout << '\t' << glfwExtensions[i] << '\n';
	}

	std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};

	if (CheckValidationLayers((unsigned int)validationLayers.size(), validationLayers.data()))
	{
		createInfo.enabledLayerCount = (unsigned int)validationLayers.size();
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
#endif

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions.data();
	createInfo.enabledLayerCount = 0;
	
	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance");
	}
}

std::vector<const char*> Application::getRequiredExtensions(unsigned int* count) {
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(count);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + (*count));


#ifdef _DEBUG
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	++ (*count);
#endif
	return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Application::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {


	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		// Message is important enough to show
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	}
	return VK_FALSE;
}

void Application::setupDebugMessenger()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

VkResult Application::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Application::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

bool Application::CheckValidationLayers(unsigned int size, const char** requested_layers)
{
	unsigned int layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	VkLayerProperties* layers = new VkLayerProperties[layerCount];

	vkEnumerateInstanceLayerProperties(&layerCount, layers);

#ifdef _DEBUG
	std::cout << "Available layers : \n";
	for (unsigned int i = 0; i < layerCount; ++i)
	{
		std::cout << '\t' << layers[i].layerName << ' : ' << layers[i].description << '\n';
	}

#endif // DEBUG


	for (unsigned int i = 0; i < size; ++i) {
		bool layerFound = false;

		for (unsigned int j = 0; j < layerCount; ++j) 
		{
			if (strcmp(requested_layers[i], layers[j].layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			delete[] layers;
			return false;
		}
	}
	delete[] layers;
	return true;
}

void Application::createSurface() {
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}

void Application::pickPhysicalDevice()
{
	std::vector<const char*> ExtentionsNeeded{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	unsigned int deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	VkPhysicalDevice* devices = new VkPhysicalDevice[deviceCount];
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
	
	std::vector<VkPhysicalDevice> suitableDevices;
	VkPhysicalDevice finalSelectedDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	unsigned int queueFamilyCount;
	bool hasExtensions = false;

#ifdef _DEBUG

	std::cout << "available devices:";
#endif // _DEBUG

	for (unsigned int i = 0; i < deviceCount; ++i)
	{
		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures);
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, nullptr);



#ifdef _DEBUG
		
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, queueFamilyProperties.data());
		
		
		std::cout << deviceProperties.deviceName << ", " << deviceProperties.deviceType << ", " << deviceProperties.apiVersion << '\n';
		std::cout << deviceFeatures.geometryShader << '\n';
		std::cout << queueFamilyCount << '\n';
		for (const VkQueueFamilyProperties& properties : queueFamilyProperties)
		{
			std::cout << properties.queueCount << ", " << properties.queueFlags << '\n';
		}

		//Any device after a Descreate Device is Found is not Logged.

#endif // _DEBUG



		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && isDeviceSuitable(devices[i], ExtentionsNeeded))
		{
			finalSelectedDevice = devices[i];
			hasExtensions = true;
			break;
		}
	}

	if (finalSelectedDevice == VK_NULL_HANDLE)
	{
		if (isDeviceSuitable(devices[deviceCount - 1], ExtentionsNeeded))
		{
			hasExtensions = true;
			finalSelectedDevice = devices[deviceCount - 1];
		}
	}

	if (!hasExtensions)
	{
		throw std::runtime_error("Failed to find Suitable Physical Rendering Device");
	}

	physicalDevice = finalSelectedDevice;

	unsigned int graphicsQueueFamilyIndex = UINT32_MAX, presentationQueueFamilyIndex = UINT32_MAX;
	delete[] devices;

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(finalSelectedDevice, &queueFamilyCount, queueFamilyProperties.data());

	unsigned int i = 0;
	unsigned int presentSupport = false;
	for (const auto& queueFamily : queueFamilyProperties) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			graphicsQueueFamilyIndex = i;
			vkGetPhysicalDeviceSurfaceSupportKHR(finalSelectedDevice, i, surface, &presentSupport);
			if (presentSupport)
			{
				presentationQueueFamilyIndex = i;
			}
			if (presentationQueueFamilyIndex != UINT32_MAX && graphicsQueueFamilyIndex != UINT32_MAX)
			{
				break;
			}
		}
		i++;
	}

	if (graphicsQueueFamilyIndex == UINT32_MAX)
	{
		throw std::runtime_error("failed to find a suitable queue family");
	}
	
	std::vector<unsigned int> indices {presentationQueueFamilyIndex, graphicsQueueFamilyIndex};
	device.Init(finalSelectedDevice, indices, ExtentionsNeeded);
}

bool Application::isDeviceSuitable(VkPhysicalDevice device, std::vector<const char*> extensionsNeeded)
{
	if 
	(
		checkDeviceExtensionSupport(device, extensionsNeeded)
	)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Application::checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*>& ExtentionsNeeded)
{
	unsigned int ExtensionCount = 0, ExtensionsFound = 0;
	
	vkEnumerateDeviceExtensionProperties(device, nullptr, &ExtensionCount, nullptr);
	std::vector<VkExtensionProperties> presentExtensions(ExtensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &ExtensionCount, presentExtensions.data());

	for (const char* reqextention : ExtentionsNeeded)
	{
		for (const VkExtensionProperties& extension : presentExtensions)
		{
			if (strcmp(reqextention, extension.extensionName) == 0)
			{
				++ExtensionsFound;
				break;
			}
		}
	}

	if (ExtensionsFound < (unsigned int)ExtentionsNeeded.size())
	{
		return false;
	}
	else
	{
		return true;
	}
}

SwapChainSupportDetails Application::querySwapChainSupport()
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}


VkSurfaceFormatKHR Application::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR Application::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}
	
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Application::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		
		actualExtent.width = my_clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = my_clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}

}

void Application::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport();

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (device.commonQueue)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = (unsigned int)device.queueIndices.size();
		createInfo.pQueueFamilyIndices = device.queueIndices.data();
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device.vulkdevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}
}

void Application::Mainloop()
{
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}
void Application::Cleanup()
{
	device.Destroy();

	vkDestroySurfaceKHR(instance, surface, nullptr);

#ifdef _DEBUG
	DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

#endif // _DEBUG


	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);

	glfwTerminate();
}
