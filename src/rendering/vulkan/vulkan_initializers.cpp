#include "vulkan_initializers.h"

namespace Avarice
{
    VkCommandPoolCreateInfo VulkanInitializers::CommandPoolCreateInfo(uint32_t _queueFamilyIndex, VkCommandPoolCreateFlags _flag)
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        commandPoolCreateInfo.queueFamilyIndex = _queueFamilyIndex;
        commandPoolCreateInfo.flags = _flag;
        return commandPoolCreateInfo;
    }

    VkCommandBufferAllocateInfo VulkanInitializers::CommandBufferAllocateInfo(VkCommandPool _pool, uint32_t _count, VkCommandBufferLevel _level)
    {
        VkCommandBufferAllocateInfo commandBufferAllocateInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        commandBufferAllocateInfo.commandPool = _pool;
        commandBufferAllocateInfo.commandBufferCount = _count;
        commandBufferAllocateInfo.level = _level;
        return commandBufferAllocateInfo;
    }

    VkPipelineShaderStageCreateInfo VulkanInitializers::PipelineShaderStageCreateInfo(VkShaderStageFlagBits _stage, VkShaderModule _shaderModule)
    {
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
        shaderStageCreateInfo.stage = _stage;
        shaderStageCreateInfo.module = _shaderModule;
        shaderStageCreateInfo.pName = "main";

        return shaderStageCreateInfo;
    }

    VkPipelineVertexInputStateCreateInfo VulkanInitializers::PipelineVertexInputStateCreateInfo()
    {
        VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

        // no vertex bindings or attributes
        pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
        pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;

        return pipelineVertexInputStateCreateInfo;
    }

    VkPipelineInputAssemblyStateCreateInfo VulkanInitializers::PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology _topology)
    {
        VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
        pipelineInputAssemblyStateCreateInfo.topology = _topology;
        pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

        return pipelineInputAssemblyStateCreateInfo;
    }

    VkPipelineRasterizationStateCreateInfo VulkanInitializers::PipelineRasterizationStateCreateInfo(VkPolygonMode _polygonMode)
    {
        VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
        pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;

        pipelineRasterizationStateCreateInfo.polygonMode = _polygonMode;
        pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
        pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
        pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

        pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.f;
        pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.f;
        pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.f;

        return pipelineRasterizationStateCreateInfo;
    }

    VkPipelineMultisampleStateCreateInfo VulkanInitializers::PipelineMultisampleStateCreateInfo()
    {
        VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};

        pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        pipelineMultisampleStateCreateInfo.minSampleShading = 1.0f;
        pipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
        pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
        pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

        return pipelineMultisampleStateCreateInfo;
    }

    VkPipelineColorBlendAttachmentState VulkanInitializers::PipelineColorBlendAttachmentState()
    {
        VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
        pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
        /* enable alpha blend
        pipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
        pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
        */
        return pipelineColorBlendAttachmentState;
    }

    VkPipelineLayoutCreateInfo VulkanInitializers::PipelineLayoutCreateInfo()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

        pipelineLayoutCreateInfo.flags = 0;
        pipelineLayoutCreateInfo.setLayoutCount = 0;
        pipelineLayoutCreateInfo.pSetLayouts = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

        return pipelineLayoutCreateInfo;
    }
}