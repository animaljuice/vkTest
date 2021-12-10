#include "GraphicPipeline.h"
#include "CommandBuffer.h"
#include "VulkanMachine.h"
#include <vulkan/vulkan.hpp>

using namespace vk_engine;

namespace vk_engine {
	struct GraphicPipelinePrivate {
		QueueInfo m_queueInfo;
		CommandBuffer m_cmdBuffer;

		GraphicPipelinePrivate(QueueInfo graphicsQueueInfo) :
			m_queueInfo(graphicsQueueInfo),
			m_cmdBuffer(graphicsQueueInfo.m_familyIndex)
		{

		}
	};
}

void GraphicPipeline::swap(GraphicPipeline& other)
{
	auto MyImpl = _m_pImpl;
	_m_pImpl = other._m_pImpl;
	other._m_pImpl = MyImpl;
}

GraphicPipeline::GraphicPipeline():
	_m_pImpl(new GraphicPipelinePrivate(VulkanMachine::instance().allocateGraphicsQueue()))
{

}

GraphicPipeline::GraphicPipeline(GraphicPipeline&& other)
{
	if (&other != this) {
		swap(other);
	}
}

GraphicPipeline& GraphicPipeline::operator=(GraphicPipeline&& other)
{
	if (&other != this) {
		swap(other);
	}

	return *this;
}

GraphicPipeline::~GraphicPipeline()
{
	delete _m_pImpl;
}