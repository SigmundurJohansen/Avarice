
#pragma once
#include "../rendering/buffer.h"
#include <vulkan/vulkan.h>
#include <memory>
#include "vulkan_utilities.h"
#include <vector>

namespace Avarice
{
    class VulkanRenderer;

    struct VulkanBuffer
    {
        VulkanBuffer(VmaAllocator *_allocator, uint64_t _bufferSize, VkBufferUsageFlags _bufferUsage, VmaMemoryUsage _vmaUsage);
        ~VulkanBuffer();

        void UploadData(int *_data, uint64_t _bufferSize);
        VkBuffer Buffer;
        VmaAllocation Allocation{nullptr};
        static void CopyBuffer(VkDevice *_device, VkCommandPool *_commandPool, VkQueue *_queue,
                               VulkanBuffer *_srcBuffer, VulkanBuffer *_dstBuffer, VkDeviceSize _size);
        static void CopyBufferToImage(VkCommandBuffer _cmd, VkImageLayout _dstImageLayout, VulkanBuffer *_srcBuffer,
                                      VkImage *_dstImage, VkExtent3D _imageExtent);

    private:
        VmaAllocator *m_allocator{nullptr};
    };

    class VulkanVertexBuffer : public VertexBuffer
    {
    public:
        explicit VulkanVertexBuffer(VulkanRenderer *_renderer);
        ~VulkanVertexBuffer() override;

        void UploadData(const std::vector<Vertex> &_vertices) override;
        void Bind() override;
        uint64_t GetCount() override { return m_count; };

    private:
        VulkanRenderer *m_renderer;
        uint64_t m_bufferSize;
        std::shared_ptr<VulkanBuffer> m_buffer{nullptr};
        uint64_t m_count = 0;
    };

    class VulkanIndexBuffer : public IndexBuffer
    {
    public:
        explicit VulkanIndexBuffer(VulkanRenderer *_renderer);
        ~VulkanIndexBuffer() override;
        void Bind() override;
        void UploadData(const std::vector<uint32_t> &_vector) override;
        uint64_t GetCount() override { return m_count; };

    private:
        VulkanRenderer *m_renderer;
        uint64_t m_bufferSize;
        std::shared_ptr<VulkanBuffer> m_buffer{nullptr};
        uint64_t m_count = 0;
    };

    class VulkanUniformBuffer : public UniformBuffer
    {
    public:
        explicit VulkanUniformBuffer(VulkanRenderer *_renderer);
        ~VulkanUniformBuffer() override;
        VkDescriptorSet GetDescriptorSet(VkDescriptorSetLayout *_descriptorSetLayout);
        void ResetDescriptorSet();
        void Bind() override;
        void UploadData(const UniformBufferObject &_object) override;

    private:
        VulkanRenderer *m_renderer;
        VkDescriptorSet m_descriptorSet{VK_NULL_HANDLE};
        uint64_t m_bufferSize;
        std::shared_ptr<VulkanBuffer> m_buffer{nullptr};
    };
}
