#pragma once
#include <vulkan/vulkan.h>

namespace Avarice
{
    class VulkanInitializers
    {
        public:
        static VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t _queueFamilyIndex, VkCommandPoolCreateFlags _flag = 0);
        static VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool _pool, uint32_t _count = 1, VkCommandBufferLevel _level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        
        // RENDER PASS INITIALIZERS

        static VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits _stage, VkShaderModule _shaderModule);
        static VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo();
        static VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology _topology);
        static VkPipelineRasterizationStateCreateInfo PipelineRasterizationStateCreateInfo(VkPolygonMode _polygonMode);
        static VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo();
        static VkPipelineColorBlendAttachmentState PipelineColorBlendAttachmentState();

        static VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo();
    };


}