#include "vulkan_shader.h"
#include "vulkan_initializers.h"
#include "vulkan_pipeline_builder.h"
#include "vulkan_types.h"
#include "vulkan_renderer.h"
#include "vulkan_buffer.h"
#include "vulkan_texture.h"

namespace Avarice
{
    VulkanShader::VulkanShader(VulkanRenderer *_renderer) : m_renderer(_renderer)
    {
    }

    VulkanShader::~VulkanShader()
    {        
        CleanPipeline();
    }
    void VulkanShader::Rebuild() {
        CleanPipeline();
        if (!m_vertexShader.empty() && !m_fragmentShader.empty())
            Load(std::move(m_vertexShader), std::move(m_fragmentShader));
    }
    void VulkanShader::Bind()
    {
        // Bind Uniforms
        for (auto& uniform : m_uniformBuffers) {
            auto descriptorSet = dynamic_cast<VulkanUniformBuffer*>(uniform.get())->GetDescriptorSet(&m_descriptorSetLayout);

            vkCmdBindDescriptorSets((VkCommandBuffer) m_renderer->GetCurrentFrame().MainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout,
                                    0, 1, &descriptorSet, 0, nullptr);
        }

        vkCmdBindDescriptorSets(m_renderer->GetCurrentFrame().MainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout,
                                1, 1, &m_texturesDescriptorSet, 0, nullptr);


        vkCmdBindPipeline(m_renderer->GetCurrentFrame().MainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    }

    void VulkanShader::AddUniformBuffer(std::shared_ptr<UniformBuffer> buffer) {
        m_uniformBuffers.push_back(buffer);
        Rebuild();
    }

    void VulkanShader::AddTexture(std::shared_ptr<Texture> texture) {
        m_textures.push_back(texture);
        Rebuild();
    }

    void VulkanShader::Load(const std::string &&_vertexShader, const std::string &&_fragmentShader)
    {
        m_vertexShader = _vertexShader;
        m_fragmentShader = _fragmentShader;

        VkShaderModule fragShader;
        if (!VulkanUtilities::LoadShaderModule(_fragmentShader, m_renderer->m_device, fragShader))
        {
            std::cout << "Failed to load fragment shader module\n";
        }
        VkShaderModule vertShader;
        if (!VulkanUtilities::LoadShaderModule(_vertexShader, m_renderer->m_device, vertShader))
        {
            std::cout << "Failed to load vertex shader module\n";
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

    void VulkanShader::BuildDescriptorSets() {
        std::vector<VkDescriptorSetLayoutBinding> bindings {GetUniformBufferLayoutBinding(0)};
        auto createDescriptorSetLayout = BuildDescriptorSetLayout(bindings);

        VK_CHECK(vkCreateDescriptorSetLayout(m_renderer->m_device, &createDescriptorSetLayout, nullptr, &m_descriptorSetLayout));

        bindings = {};

        for (size_t i = 0; i < m_textures.size(); i++) {
            bindings.push_back(GetTextureLayoutBinding(i));
        }

        createDescriptorSetLayout = BuildDescriptorSetLayout(bindings);
        VK_CHECK(vkCreateDescriptorSetLayout(m_renderer->m_device, &createDescriptorSetLayout, nullptr,
                                             &m_textureSetLayout));

        VkDescriptorSetAllocateInfo allocateInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        allocateInfo.descriptorPool = m_renderer->m_descriptorPool;
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pSetLayouts = &m_textureSetLayout;
        VK_CHECK(vkAllocateDescriptorSets(m_renderer->m_device, &allocateInfo, &m_texturesDescriptorSet));

        for (size_t i = 0; i < m_textures.size(); i++) {
            auto texture = m_textures[i];
            dynamic_cast<VulkanTexture *>(texture.get())->WriteToDescriptorSet(m_texturesDescriptorSet, i);
        }
    }
}