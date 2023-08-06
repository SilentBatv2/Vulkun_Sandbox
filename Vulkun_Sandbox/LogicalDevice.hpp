#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vector>

class LogicalDevice
{
public:
	VkDevice vulkdevice;
	VkPhysicalDevice GPU;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	std::vector<unsigned int> queueIndices;
	bool commonQueue;


	LogicalDevice();
	void Init(VkPhysicalDevice selectedGPU, std::vector<unsigned int> indeces, std::vector<const char*>& ExtentionsNeeded);
	void Destroy();
};