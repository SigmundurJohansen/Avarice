
#include <iostream>
#include "vulkan_pipeline_builder.h"

namespace Avarice
{
    VkPipeline VulkanPipelineBuilder::BuildPipeline(VkDevice _device, VkRenderPass _pass)
    {
        VkPipelineViewportStateCreateInfo viewportState{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
        viewportState.viewportCount = 1;
        viewportState.pViewports = &m_viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &m_scissor;

        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
        colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendStateCreateInfo.attachmentCount = 1;
        colorBlendStateCreateInfo.pAttachments = &m_colorBlendAttachment;

        VkGraphicsPipelineCreateInfo pipelineCreateInfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
        pipelineCreateInfo.stageCount = m_shaderStages.size();
        pipelineCreateInfo.pStages = m_shaderStages.data();
        pipelineCreateInfo.pVertexInputState = &m_vertexInputInfo;
        pipelineCreateInfo.pInputAssemblyState = &m_inputAssembly;
        pipelineCreateInfo.pViewportState = &viewportState;
        pipelineCreateInfo.pRasterizationState = &m_rasterizer;
        pipelineCreateInfo.pMultisampleState = &m_multisampling;
        pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
        pipelineCreateInfo.layout = m_pipelineLayout;
        pipelineCreateInfo.renderPass = _pass;
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.pDepthStencilState = &m_depthStencil;

        VkPipeline newPipeline;

        if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &newPipeline) != VK_SUCCESS)
        {
            std::cout << "Failed to create pipeline\n";
            return VK_NULL_HANDLE;
        }

        return newPipeline;
    }
}