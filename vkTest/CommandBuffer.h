#pragma once

#include <vulkan/vulkan.hpp>

namespace vk_engine {
	class CommandBuffer
	{
		VkCommandBuffer _m_commandBuffer = VK_NULL_HANDLE;
		uint32_t _m_familyIndex;

	public:
		CommandBuffer(uint32_t familyIndex);
		CommandBuffer(const CommandBuffer& other) = delete;
		CommandBuffer &operator=(const CommandBuffer& other) = delete;
		virtual ~CommandBuffer();
	};
}

