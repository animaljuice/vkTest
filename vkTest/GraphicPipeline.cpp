#include "GraphicPipeline.h"
#include <vulkan/vulkan.hpp>

using namespace vk_engine;

namespace vk_engine {
	struct GraphicPipelinePrivate {
		VkQueue m_graphicsQueue;
		GraphicPipelinePrivate(VkQueue graphicsQueue) :
			m_graphicsQueue(graphicsQueue) {

		}
	};
}

void GraphicPipeline::swap(GraphicPipeline& other)
{
	auto MyImpl = _m_pImpl;
	_m_pImpl = other._m_pImpl;
	other._m_pImpl = MyImpl;
}

GraphicPipeline::GraphicPipeline(void *internalDescription):
	_m_pImpl(new GraphicPipelinePrivate(reinterpret_cast<VkQueue>(internalDescription)))
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