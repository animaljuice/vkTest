#pragma once

namespace vk_engine {
	class VulkanMachine;
	class GraphicPipeline
	{
		struct GraphicPipelinePrivate* _m_pImpl = nullptr;
		void swap(GraphicPipeline& other);
		GraphicPipeline(void* internalDescription);
		friend class VulkanMachine;

	public:
		GraphicPipeline(const GraphicPipeline& other) = delete;
		GraphicPipeline &operator=(const GraphicPipeline& other) = delete;
		GraphicPipeline(GraphicPipeline&& other);
		GraphicPipeline& operator=(GraphicPipeline&& other);
		virtual ~GraphicPipeline();

	};
}

