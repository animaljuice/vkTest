#pragma once

namespace vk_engine {
	class GraphicPipeline
	{
		struct GraphicPipelinePrivate* _m_pImpl = nullptr;
		void swap(GraphicPipeline& other);

	public:
		GraphicPipeline();
		GraphicPipeline(const GraphicPipeline& other) = delete;
		GraphicPipeline &operator=(const GraphicPipeline& other) = delete;
		GraphicPipeline(GraphicPipeline&& other) noexcept;
		GraphicPipeline& operator=(GraphicPipeline&& other) noexcept;
		virtual ~GraphicPipeline();

	};
}

