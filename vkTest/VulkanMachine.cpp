#include "VulkanMachine.h"
#include <iostream>
#include <vulkan/vulkan.hpp>

using namespace vk_engine;

namespace vk_engine {
	struct VulkanMachinePrivate {
		VkInstance m_inst = nullptr;
		VkDevice m_device = nullptr;
		VkPhysicalDevice m_physDev = nullptr;

		VkAllocationCallbacks m_callBacks;

		std::vector<uint32_t> graphicQueueIndexes;
		std::vector<uint32_t> transferQueueIndexes;
		std::vector<uint32_t> computeQueueIndexes;
		std::vector<uint32_t> sparseQueueIndexes;
		std::vector<uint32_t> protectedQueueIndexes;
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

	for (size_t famIndex = 0; famIndex < familyProps.size(); ++famIndex)
	{
		if (familyProps[famIndex].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			_m_pImpl->computeQueueIndexes.push_back(famIndex);
		}
		if (familyProps[famIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			_m_pImpl->graphicQueueIndexes.push_back(famIndex);
		}
		if (familyProps[famIndex].queueFlags & VK_QUEUE_TRANSFER_BIT) {
			_m_pImpl->transferQueueIndexes.push_back(famIndex);
		}
		if (familyProps[famIndex].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
			_m_pImpl->sparseQueueIndexes.push_back(famIndex);
		}
		if (familyProps[famIndex].queueFlags & VK_QUEUE_PROTECTED_BIT) {
			_m_pImpl->protectedQueueIndexes.push_back(famIndex);
		}
	}
	/*uint32_t layerCount;	
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> lps(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, lps.data());*/

	//VkDeviceQueueCreateInfo dqci;
	//dqci.flags = 0;
	//dqci.pNext = nullptr;
	//dqci.pQueuePriorities

	//VkDeviceCreateInfo dci;
	//dci.enabledExtensionCount = 0;
	//dci.enabledLayerCount = 0;
	//dci.flags = 0;
	//dci.pEnabledFeatures = nullptr;
	//dci.pNext = nullptr;
	//dci.ppEnabledExtensionNames = nullptr;
	//dci.ppEnabledLayerNames = nullptr;
	//dci.pQueueCreateInfos
	//vkCreateDevice(_m_pImpl->m_physDev, )

	std::cout << "asdqwe";
}

VulkanMachine::~VulkanMachine()
{
	vkDestroyInstance(_m_pImpl->m_inst, &_m_pImpl->m_callBacks);
	delete _m_pImpl;
}
