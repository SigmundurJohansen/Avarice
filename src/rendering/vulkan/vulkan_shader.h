#pragma once
#include "../rendering/shader.h"
#include "../rendering/texture.h"
#include "../rendering/buffer.h"
#include "vulkan_utilities.h"

namespace Avarice
{
    class VulkanRenderer;
    class VulkanShader : public Shader
    {
    public:
        explicit VulkanShader(VulkanRenderer *_renderer);
        ~VulkanShader() override;
        void Rebuild();
        void Bind() override;
        void Load(const std::string &&_vertexShader, const std::string &&_fragmentShader) override;
        void AddUniformBuffer(std::shared_ptr<UniformBuffer> buffer) override;
        void AddTexture(std::shared_ptr<Texture> texture) override;

    private:
        void CleanPipeline();
        void BuildDescriptorSets();
        VulkanRenderer *m_renderer;
        // vulkan handles
        VkDescriptorSetLayout m_descriptorSetLayout{VK_NULL_HANDLE};
        VkDescriptorSetLayout m_textureSetLayout{VK_NULL_HANDLE};
        VkDescriptorSet m_texturesDescriptorSet{VK_NULL_HANDLE};

        VkPipelineLayout m_pipelineLayout{VK_NULL_HANDLE};
        VkPipeline m_pipeline{VK_NULL_HANDLE};

        // FILE LOCATIONS FOR REBUILDING
        std::string m_vertexShader;
        std::string m_fragmentShader;

        std::vector<std::shared_ptr<UniformBuffer>> m_uniformBuffers{};
        std::vector<std::shared_ptr<Texture>> m_textures{};
    };
}