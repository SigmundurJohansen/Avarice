#pragma once
#include "vulkan_buffer.h"
#include "vulkan_device.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Avarice
{
    class VulkanModel
    {
    public:
        struct Vertex
        {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            bool operator==(const Vertex &other) const
            {
                return position == other.position && color == other.color && normal == other.normal &&
                       uv == other.uv;
            }
        };

        struct Builder
        {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void loadModel(const std::string &filepath);
        };

        VulkanModel(VulkanDevice &device, const VulkanModel::Builder &builder);
        ~VulkanModel();

        VulkanModel(const VulkanModel &) = delete;
        VulkanModel &operator=(const VulkanModel &) = delete;

        static std::unique_ptr<VulkanModel> createModelFromFile(
            VulkanDevice &device, const std::string &filepath);

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);

        VulkanDevice &m_device;

        std::unique_ptr<VulkanBuffer> m_vertexBuffer;
        uint32_t m_vertexCount;

        bool m_hasIndexBuffer = false;
        std::unique_ptr<VulkanBuffer> m_indexBuffer;
        uint32_t m_indexCount;
    };
}