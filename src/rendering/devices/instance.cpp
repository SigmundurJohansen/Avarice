#include "instance.h"
#include <iomanip>
#include "../graphics.h"

namespace Avarice
{
	void Instance::FvkCmdPushDescriptorSetKHR(VkDevice device, VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set,
											  uint32_t descriptorWriteCount, const VkWriteDescriptorSet *pDescriptorWrites)
	{
		auto func = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(vkGetDeviceProcAddr(device, "vkCmdPushDescriptorSetKHR"));
		if (func)
			func(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
	}

	uint32_t Instance::FindMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties *deviceMemoryProperties, const VkMemoryRequirements *memoryRequirements,
										   VkMemoryPropertyFlags requiredProperties)
	{
		for (uint32_t i = 0; i < deviceMemoryProperties->memoryTypeCount; ++i)
		{
			if (memoryRequirements->memoryTypeBits & (1 << i))
			{
				if ((deviceMemoryProperties->memoryTypes[i].propertyFlags & requiredProperties) == requiredProperties)
				{
					return i;
				}
			}
		}
		throw std::runtime_error("Couldn't find a proper memory type");
	}

	Instance::Instance()
	{
		CreateInstance();
		CreateDebugMessenger();
	}

	Instance::~Instance()
	{
		vkDestroyInstance(instance, nullptr);
	}

	bool Instance::CheckValidationLayerSupport() const
	{
		uint32_t instanceLayerPropertyCount;
		vkEnumerateInstanceLayerProperties(&instanceLayerPropertyCount, nullptr);
		std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerPropertyCount);
		vkEnumerateInstanceLayerProperties(&instanceLayerPropertyCount, instanceLayerProperties.data());

		for (const auto &layerName : ValidationLayers)
		{
			bool layerFound = false;

			for (const auto &layerProperties : instanceLayerProperties)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}
			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}


	void Instance::CreateInstance()
	{
		//const auto &engineVersion = Engine::Get()->GetVersion();
		//const auto &appVersion = Engine::Get()->GetApp()->GetVersion();
		//const auto &appName = Engine::Get()->GetApp()->GetName();

		//Graphics::CheckVk(volkInitialize());

		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		//applicationInfo.pApplicationName = appName.c_str();
		//applicationInfo.applicationVersion = VK_MAKE_VERSION(appVersion.major, appVersion.minor, appVersion.patch);
		applicationInfo.pEngineName = "Avarice";
		//applicationInfo.engineVersion = VK_MAKE_VERSION(engineVersion.major, engineVersion.minor, engineVersion.patch);
		//applicationInfo.apiVersion = volkGetInstanceVersion() >= VK_API_VERSION_1_1 ? VK_API_VERSION_1_1 : VK_MAKE_VERSION(1, 0, 57);

		if (enableValidationLayers && !CheckValidationLayerSupport())
		{
			enableValidationLayers = false;
		}

		auto extensions = GetExtensions();

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

		Graphics::CheckVk(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));
	}

	void Instance::LogVulkanLayers(const std::vector<VkLayerProperties> &layerProperties)
	{
		std::stringstream ss;

		ss << "Instance Layers: ";
		for (const auto &layer : layerProperties)
			ss << layer.layerName << ", ";

		ss << "\n\n";
		//Log::Out(ss.str());
	}
}
