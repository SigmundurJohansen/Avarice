#pragma once
#include "vulkan_renderer.h"
#include "../rendering/shader.h"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>

namespace Avarice
{
    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 color;

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{};

            return bindingDescription;
        }
    };

    const std::vector<Vertex> vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };

    class VertexInput
    {
        public:
        VkVertexInputBindingDescription bindingDescription{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX};
    };

    class VulkanShader : public Shader
    {
    public:
        explicit VulkanShader(VulkanRenderer* _renderer);
        ~VulkanShader() override;
        void Bind() override;
        void Load(const std::string &&_vertexShader, const std::string &&_fragmentShader) override;
        private:
        VulkanRenderer* m_renderer{nullptr};
        // vulkan handles
        VkPipelineLayout m_pipelineLayout { VK_NULL_HANDLE };
        VkPipeline m_pipeline { VK_NULL_HANDLE };
    };
}