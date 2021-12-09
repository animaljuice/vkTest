#include "VulkanMachine.h"
#include <iostream>
#include <vulkan/vulkan.hpp>

using namespace vk_engine;

namespace vk_engine {
	struct VulkanMachinePrivate {
		VkInstance m_inst = nullptr;
		VkDevice m_device = nullptr;
		VkPhysicalDevice m_physDev = nullptr;

		std::vector<uint32_t> m_graphicQueueIndexes{};
		std::vector<uint32_t> m_transferQueueIndexes{};
		std::vector<uint32_t> m_computeQueueIndexes{};
		std::vector<uint32_t> m_sparseQueueIndexes{};
		std::vector<uint32_t> m_protectedQueueIndexes{};

		std::vector<uint32_t> m_queuesCount{};
		std::vector<uint32_t> m_acquiredQueues{};

		std::vector<VkCommandPool> m_cmdPools;
	};
}

VulkanMachine::VulkanMachine(const std::string &appName):
	_m_pImpl(new VulkanMachinePrivate)
{
	VkApplicationInfo appInf;
	appInf.apiVersion = 0;
	appInf.applicationVersion = 0;
	appInf.engineVersion = 0;
	appInf.pApplicationName = appName.c_str();
	appInf.pEngineName = "vk_engine";
	appInf.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	appInf.pNext = nullptr;

	VkInstanceCreateInfo ici;
	ici.enabledExtensionCount = 0;
	ici.enabledLayerCount = 0;
	ici.flags = 0;
	ici.pApplicationInfo = &appInf;
	ici.ppEnabledExtensionNames = nullptr;
	ici.ppEnabledLayerNames = nullptr;
	ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	ici.pNext = nullptr;
	auto createInstRes = vkCreateInstance(&ici, nullptr, &_m_pImpl->m_inst);

	uint32_t deviceCount;
	auto phRes = vkEnumeratePhysicalDevices(_m_pImpl->m_inst, &deviceCount, nullptr);
	std::vector<VkPhysicalDevice> physDevs(deviceCount);
	phRes = vkEnumeratePhysicalDevices(_m_pImpl->m_inst, &deviceCount, physDevs.data());
	_m_pImpl->m_physDev = physDevs.front();

	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(_m_pImpl->m_physDev, &deviceProperties);
	vkGetPhysicalDeviceFeatures(_m_pImpl->m_physDev, &deviceFeatures);

	uint32_t familyPropCount;
	vkGetPhysicalDeviceQueueFamilyProperties(_m_pImpl->m_physDev, &familyPropCount, nullptr);
	std::vector<VkQueueFamilyProperties> familyProps(familyPropCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_m_pImpl->m_physDev, &familyPropCount, familyProps.data());

	_m_pImpl->m_queuesCount.resize(familyProps.size());
	_m_pImpl->m_acquiredQueues.resize(familyProps.size());
	_m_pImpl->m_cmdPools.resize(familyProps.size());

	for (uint32_t famIndex = 0; famIndex < familyProps.size(); ++famIndex)
	{
		_m_pImpl->m_queuesCount[famIndex] = familyProps[famIndex].queueCount;
		if (familyProps[famIndex].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			_m_pImpl->m_computeQueueIndexes.push_back(famIndex);
		}
		if (familyProps[famIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			_m_pImpl->m_graphicQueueIndexes.push_back(famIndex);
		}
		if (familyProps[famIndex].queueFlags & VK_QUEUE_TRANSFER_BIT) {
			_m_pImpl->m_transferQueueIndexes.push_back(famIndex);
		}
		if (familyProps[famIndex].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
			_m_pImpl->m_sparseQueueIndexes.push_back(famIndex);
		}
		if (familyProps[famIndex].queueFlags & VK_QUEUE_PROTECTED_BIT) {
			_m_pImpl->m_protectedQueueIndexes.push_back(famIndex);
		}
	}

	/*uint32_t layerCount;	
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> lps(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, lps.data());*/

	std::vector<VkDeviceQueueCreateInfo> dqcis(_m_pImpl->m_queuesCount.size());
	for (uint32_t familyIndex = 0; familyIndex < dqcis.size(); familyIndex++)
	{
		auto queueCount = _m_pImpl->m_queuesCount[familyIndex];
		std::vector<float> priorities(queueCount);
		for (uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++)
		{
			priorities[queueIndex] = queueIndex / float(queueCount - 1);
		}
		
		auto& dqci = dqcis[familyIndex];
		dqci.flags = 0;
		dqci.pNext = nullptr;
		dqci.pQueuePriorities = priorities.data();
		dqci.queueCount = queueCount;
		dqci.queueFamilyIndex = familyIndex;
		dqci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	}

	VkDeviceCreateInfo dci;
	dci.enabledExtensionCount = 0;
	dci.enabledLayerCount = 0;
	dci.flags = 0;
	dci.pEnabledFeatures = nullptr;
	dci.pNext = nullptr;
	dci.ppEnabledExtensionNames = nullptr;
	dci.ppEnabledLayerNames = nullptr;
	dci.pQueueCreateInfos = dqcis.data();
	dci.queueCreateInfoCount = uint32_t(dqcis.size());
	dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	auto dcRes = vkCreateDevice(_m_pImpl->m_physDev, &dci, nullptr, &_m_pImpl->m_device);

	for (size_t familyIndex = 0; familyIndex < _m_pImpl->m_cmdPools.size(); familyIndex++)
	{
		VkCommandPoolCreateInfo cpci;
		cpci.flags = 0;
		cpci.pNext = VK_NULL_HANDLE;
		cpci.queueFamilyIndex = familyIndex;

		vkCreateCommandPool(_m_pImpl->m_device, &cpci, VK_NULL_HANDLE, &_m_pImpl->m_cmdPools[familyIndex]);
	}

	
}

VulkanMachine::~VulkanMachine()
{
	for (size_t familyIndex = 0; familyIndex < _m_pImpl->m_cmdPools.size(); familyIndex++)
	{
		vkDestroyCommandPool(_m_pImpl->m_device, _m_pImpl->m_cmdPools[familyIndex], VK_NULL_HANDLE);
	}

	vkDestroyDevice(_m_pImpl->m_device, VK_NULL_HANDLE);
	vkDestroyInstance(_m_pImpl->m_inst, VK_NULL_HANDLE);
	delete _m_pImpl;
}

GraphicPipeline VulkanMachine::createGP()
{
	bool found = false;
	for (auto grapFamIndex : _m_pImpl->m_graphicQueueIndexes) {
		if (_m_pImpl->m_acquiredQueues[grapFamIndex] < _m_pImpl->m_queuesCount[grapFamIndex]) {
			++_m_pImpl->m_acquiredQueues[grapFamIndex];
			found = true;
			break;
		}
	}

	if (found) {
		VkQueue resDesc;
		vkGetDeviceQueue(_m_pImpl->m_device, 0, 0, &resDesc);
		return GraphicPipeline(resDesc);
	}
	else {
		throw std::exception("have not free graphics queue anymore");
	}
}
