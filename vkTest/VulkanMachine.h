#pragma once

#include <string>
#include <vulkan/vulkan.hpp>

#include "GraphicPipeline.h"

namespace vk_engine {

	struct QueueFamilyInfo {
		VkQueueFlags m_flags = 0;
		VkCommandPool m_cmdPool = VK_NULL_HANDLE;
		uint32_t m_freeQueueCount = 0;
	};

	struct QueueInfo
	{
		VkQueue m_queue;
		uint32_t m_familyIndex;
	};

class VulkanMachine
{
	struct VulkanMachinePrivate *_m_pImpl;
	VulkanMachine(const std::string& appName);
	virtual ~VulkanMachine();

public:
	VulkanMachine& operator=(const VulkanMachine& other) = delete;
	VulkanMachine(const VulkanMachine& other) = delete;

	QueueInfo allocateGraphicsQueue();
	uint32_t familyCount() const;
	QueueFamilyInfo getFamInfo(uint32_t index) const;
	VkDevice getDevice() const;

	static VulkanMachine& instance();
};
}