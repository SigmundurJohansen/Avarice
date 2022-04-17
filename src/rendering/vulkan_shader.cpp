#include "vulkan_shader.h"
#include "vulkan_utilities.h"
#include "vulkan_initializers.h"
#include "vulkan_pipeline_builder.h"

namespace Avarice
{
    VulkanShader::VulkanShader(VulkanRenderer *_renderer) : m_renderer(_renderer)
    {
    }

    VulkanShader::~VulkanShader()
    {        
        vkDestroyPipeline(m_renderer->m_device, m_pipeline, nullptr);
        vkDestroyPipelineLayout(m_renderer->m_device, m_pipelineLayout, nullptr);
    }
    void VulkanShader::Bind()
    {
        vkCmdBindPipeline(m_renderer->m_mainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    }

    void VulkanShader::Load(const std::string &&_vertexShader, const std::string &&_fragmentShader)
    {

        VkShaderModule vertShader;
        if (!VulkanUtilities::LoadShaderModule(_vertexShader, m_renderer->m_device, vertShader))
        {
            std::cout << "Failed to load vertex shader module\n";
        }

        VkShaderModule fragShader;
        if (!VulkanUtilities::LoadShaderModule(_fragmentShader, m_renderer->m_device, fragShader))
        {
            std::cout << "Failed to load fragment shader module\n";
        }
        auto pipelineLayoutInfo = VulkanInitializers::PipelineLayoutCreateInfo();
        VK_CHECK(vkCreatePipelineLayout(m_renderer->m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout));

        VulkanPipelineBuilder pipelineBuilder;
        pipelineBuilder.m_shaderStages.push_back(
            VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertShader));
        pipelineBuilder.m_shaderStages.push_back(
            VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragShader));

        pipelineBuilder.m_vertexInputInfo = VulkanInitializers::PipelineVertexInputStateCreateInfo();
        pipelineBuilder.m_inputAssembly = VulkanInitializers::PipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        // build the viewport
        pipelineBuilder.m_viewport = {
            .x = 0.f,
            .y = 0.f,
            .width = static_cast<float>(m_renderer->m_windowExtent.width),
            .height = static_cast<float>(m_renderer->m_windowExtent.height),
            .minDepth = 0.f,
            .maxDepth = 1.f};

        pipelineBuilder.m_scissor = {
            .offset = {0, 0},
            .extent = m_renderer->m_windowExtent};

        pipelineBuilder.m_rasterizer = VulkanInitializers::PipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);
        pipelineBuilder.m_multisampling = VulkanInitializers::PipelineMultisampleStateCreateInfo();
        pipelineBuilder.m_colorBlendAttachment = VulkanInitializers::PipelineColorBlendAttachmentState();
        pipelineBuilder.m_pipelineLayout = m_pipelineLayout;

        m_pipeline = pipelineBuilder.BuildPipeline(m_renderer->m_device, m_renderer->m_renderPass);

        vkDestroyShaderModule(m_renderer->m_device, fragShader, nullptr);
        vkDestroyShaderModule(m_renderer->m_device, vertShader, nullptr);
    }

}