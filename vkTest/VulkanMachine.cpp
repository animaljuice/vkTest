#include "VulkanMachine.h"
#include <iostream>

using namespace vk_engine;

std::string applicationName("default app name");

namespace vk_engine {
	struct VulkanMachinePrivate {
		VkInstance m_inst = nullptr;
		VkDevice m_device = nullptr;
		VkPhysicalDevice m_physDev = nullptr;

		std::vector<QueueFamilyInfo> m_familyInfos;
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

	_m_pImpl->m_familyInfos.resize(familyProps.size());
	for (uint32_t famIndex = 0; famIndex < familyProps.size(); ++famIndex) {
		_m_pImpl->m_familyInfos[famIndex].m_flags = familyProps[famIndex].queueFlags;
		_m_pImpl->m_familyInfos[famIndex].m_freeQueueCount = familyProps[famIndex].queueCount;
	}

	/*uint32_t layerCount;	
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> lps(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, lps.data());*/

	std::vector<VkDeviceQueueCreateInfo> dqcis(_m_pImpl->m_familyInfos.size());
	for (uint32_t familyIndex = 0; familyIndex < dqcis.size(); familyIndex++)
	{
		auto queueCount = _m_pImpl->m_familyInfos[familyIndex].m_freeQueueCount;
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

	for (size_t familyIndex = 0; familyIndex < _m_pImpl->m_familyInfos.size(); familyIndex++)
	{
		VkCommandPoolCreateInfo cpci;
		cpci.flags = 0;
		cpci.pNext = VK_NULL_HANDLE;
		cpci.queueFamilyIndex = familyIndex;

		vkCreateCommandPool(_m_pImpl->m_device, &cpci, VK_NULL_HANDLE, &_m_pImpl->m_familyInfos[familyIndex].m_cmdPool);
	}

	std::cout << "asdqwe";
}

VulkanMachine::~VulkanMachine()
{
	for (size_t familyIndex = 0; familyIndex < _m_pImpl->m_familyInfos.size(); familyIndex++)
	{
		vkDestroyCommandPool(_m_pImpl->m_device, _m_pImpl->m_familyInfos[familyIndex].m_cmdPool, VK_NULL_HANDLE);
	}

	vkDestroyDevice(_m_pImpl->m_device, VK_NULL_HANDLE);
	vkDestroyInstance(_m_pImpl->m_inst, VK_NULL_HANDLE);
	delete _m_pImpl;
}

QueueInfo VulkanMachine::allocateGraphicsQueue()
{
	bool found = false;
	uint32_t familyIndex = 0, queueIndex = 0;
	for (uint32_t famIndex = 0; famIndex < _m_pImpl->m_familyInfos.size(); ++famIndex) {
		if (_m_pImpl->m_familyInfos[famIndex].m_freeQueueCount > 0 && _m_pImpl->m_familyInfos[famIndex].m_flags & VK_QUEUE_GRAPHICS_BIT) {
			familyIndex = famIndex;
			queueIndex = _m_pImpl->m_familyInfos[famIndex].m_freeQueueCount--;
			found = true;
			break;
		}
	}

	if (found) {
		QueueInfo res;
		res.m_familyIndex = familyIndex;
		vkGetDeviceQueue(_m_pImpl->m_device, familyIndex, queueIndex, &res.m_queue);
		return res;
	}
	else {
		throw std::exception("have not free graphics queue anymore");
	}
}

uint32_t VulkanMachine::familyCount() const
{
	return _m_pImpl->m_familyInfos.size();
}

QueueFamilyInfo VulkanMachine::getFamInfo(uint32_t index) const
{
	return _m_pImpl->m_familyInfos.at(index);
}

VkDevice VulkanMachine::getDevice() const
{
	return _m_pImpl->m_device;
}

VulkanMachine& VulkanMachine::instance()
{
	static VulkanMachine inst(applicationName);
	return inst;
}
