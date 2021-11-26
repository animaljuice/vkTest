#pragma once

#include "GraphicPipeline.h"
#include <string>

namespace vk_engine {
class VulkanMachine
{
	struct VulkanMachinePrivate *_m_pImpl;

public:
	VulkanMachine(const std::string& appName);
	VulkanMachine& operator=(const VulkanMachine& other) = delete;
	VulkanMachine(const VulkanMachine& other) = delete;
	virtual ~VulkanMachine();

	GraphicPipeline createGP() const;

};
}