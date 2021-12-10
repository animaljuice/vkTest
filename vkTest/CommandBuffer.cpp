#include "CommandBuffer.h"
#include "VulkanMachine.h"

using namespace vk_engine;

CommandBuffer::CommandBuffer(uint32_t familyIndex):
	_m_familyIndex(familyIndex)
{
	VkCommandBufferAllocateInfo cbai;
	cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cbai.commandBufferCount = 1;
	cbai.commandPool = VulkanMachine::instance().getFamInfo(_m_familyIndex).m_cmdPool;
	cbai.pNext = VK_NULL_HANDLE;
	cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	vkAllocateCommandBuffers(VulkanMachine::instance().getDevice(), &cbai, &_m_commandBuffer);
}

CommandBuffer::~CommandBuffer()
{
	auto &vkMach = VulkanMachine::instance();
	vkFreeCommandBuffers(vkMach.getDevice(), vkMach.getFamInfo(_m_familyIndex).m_cmdPool, 1, &_m_commandBuffer);
}
