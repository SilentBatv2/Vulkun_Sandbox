#include"LogicalDevice.hpp"

LogicalDevice::LogicalDevice()
{
	GPU = VK_NULL_HANDLE;
	vulkdevice = VK_NULL_HANDLE;
	graphicsQueue = VK_NULL_HANDLE;
	presentQueue = VK_NULL_HANDLE;
	commonQueue = false;
}

void LogicalDevice::Init(VkPhysicalDevice physicalDevice, std::vector<unsigned int> indices, std::vector<const char*>& ExtentionsNeeded)
{
	float queuePriority = 1.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	
	for (uint32_t queueFamily : indices) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures features{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = (unsigned int)queueCreateInfos.size();

	createInfo.enabledExtensionCount = (unsigned int)ExtentionsNeeded.size();
	createInfo.ppEnabledExtensionNames = ExtentionsNeeded.data();

	createInfo.pEnabledFeatures = &features;

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &vulkdevice) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	if (indices[0] == indices[1])
	{
		commonQueue = true;
		queueIndices.emplace_back(indices[0]);
	}
	else
	{
		for (int i = 0; i < indices.size(); ++i)
		{
			queueIndices.emplace_back(indices[i]);
		}
	}

	vkGetDeviceQueue(vulkdevice, indices[0], 0, &graphicsQueue);
	vkGetDeviceQueue(vulkdevice, indices[1], 0, &presentQueue);

}

void LogicalDevice::Destroy()
{
	vkDestroyDevice(vulkdevice, nullptr);
}