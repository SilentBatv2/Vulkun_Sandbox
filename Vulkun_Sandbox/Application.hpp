#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <stdexcept>
#include <iostream>
#include <vector>
#include "LogicalDevice.hpp"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities{};
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class Application 
{
	GLFWwindow* window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice;
	LogicalDevice device;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;

	void InitWindow();
	void InitVulkan();
	void Mainloop();
	void Cleanup();
	void CreateInstance();
	std::vector<const char*> getRequiredExtensions(unsigned int* count);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
	void setupDebugMessenger();
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	bool CheckValidationLayers(unsigned int size, const char** requested_layers);
	void createSurface();
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device, std::vector<const char*> extensionsNeeded);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*>& ExtentionsNeeded);
	SwapChainSupportDetails querySwapChainSupport();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void createSwapChain();
//	unsigned int SetDeviceScore(VkPhysicalDeviceProperties properties, VkPhysicalDeviceFeatures features);
public:
	Application();
	void run()
	{
		InitWindow();
		InitVulkan();
		Mainloop();
		Cleanup();
	}
};