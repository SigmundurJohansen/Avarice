#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace Avarice {
    class VulkanPipelineBuilder {
    public:
        std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
        VkPipelineVertexInputStateCreateInfo m_vertexInputInfo;
        VkPipelineInputAssemblyStateCreateInfo m_inputAssembly;
        VkViewport m_viewport;
        VkRect2D m_scissor;

        VkPipelineRasterizationStateCreateInfo m_rasterizer;
        VkPipelineColorBlendAttachmentState m_colorBlendAttachment;
        VkPipelineMultisampleStateCreateInfo m_multisampling;
        VkPipelineLayout m_pipelineLayout;

        VkPipeline BuildPipeline(VkDevice _device, VkRenderPass _pass);
    };
}