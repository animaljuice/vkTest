#include "GraphicPipeline.h"
#include <vulkan/vulkan.hpp>

using namespace vk_engine;

namespace vk_engine {
	struct GraphicPipelinePrivate {
		
	};
}

void GraphicPipeline::swap(GraphicPipeline& other)
{
	auto MyImpl = _m_pImpl;
	_m_pImpl = other._m_pImpl;
	other._m_pImpl = MyImpl;
}

GraphicPipeline::GraphicPipeline():
	_m_pImpl(new GraphicPipelinePrivate)
{
}

GraphicPipeline::GraphicPipeline(GraphicPipeline&& other) noexcept
{
	if (&other != this) {
		swap(other);
	}
}

GraphicPipeline& GraphicPipeline::operator=(GraphicPipeline&& other) noexcept
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