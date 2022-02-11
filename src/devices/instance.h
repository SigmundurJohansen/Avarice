#pragma once

#include <vector>
#include "engine.h"
//#include <volk.h>

namespace Avarice
{
	class Instance
	{
		friend class Graphics;

	public:
		Instance();
		~Instance();

		static void FvkCmdPushDescriptorSetKHR(VkDevice device, VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set,
											   uint32_t descriptorWriteCount, const VkWriteDescriptorSet *pDescriptorWrites);

		static uint32_t FindMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties *deviceMemoryProperties, const VkMemoryRequirements *memoryRequirements,
											VkMemoryPropertyFlags requiredProperties);

		operator const VkInstance &() const { return instance; }

		bool GetEnableValidationLayers() const { return enableValidationLayers; }
		const VkInstance &GetInstance() const { return instance; }

		static const std::vector<const char *> ValidationLayers;

	private:
		bool CheckValidationLayerSupport() const;
		std::vector<const char *> GetExtensions() const;
		void CreateInstance();
		void CreateDebugMessenger();

		static void LogVulkanLayers(const std::vector<VkLayerProperties> &layerProperties);

		bool enableValidationLayers = false;

		VkInstance instance = VK_NULL_HANDLE;
	};
}
