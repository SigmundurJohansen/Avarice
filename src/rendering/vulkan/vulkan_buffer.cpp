#include "vulkan_buffer.h"
#include "vulkan_renderer.h"
#include <memory>
#include <vector>
#include <cstring>
#include "vulkan_utilities.h"
#include <vector>

namespace Avarice
{
    VulkanBuffer::VulkanBuffer(VmaAllocator *_allocator, uint64_t _bufferSize, VkBufferUsageFlags _bufferUsage, VmaMemoryUsage _vmaUsage) : m_allocator(_allocator)
    {
        VkBufferCreateInfo bufferCreateInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferCreateInfo.size = _bufferSize;
        bufferCreateInfo.usage = _bufferUsage;

        VmaAllocationCreateInfo vmaAllocationCreateInfo{};
        vmaAllocationCreateInfo.usage = _vmaUsage;

        // allocate the buffer
        VK_CHECK(vmaCreateBuffer(*m_allocator, &bufferCreateInfo, &vmaAllocationCreateInfo,
                                 &Buffer,
                                 &Allocation,
                                 nullptr));
    }

    VulkanBuffer::~VulkanBuffer()
    {
        vmaDestroyBuffer(*m_allocator, Buffer, Allocation);
    }

    void VulkanBuffer::UploadData(int *_data, uint64_t _bufferSize)
    {
        void *tempData;
        vmaMapMemory(*m_allocator, Allocation, &tempData);
        memcpy(tempData, _data, _bufferSize);
        vmaUnmapMemory(*m_allocator, Allocation);
    }

    void VulkanBuffer::CopyBuffer(VkDevice *_device, VkCommandPool *_commandPool, VkQueue *_queue,
                                  VulkanBuffer *_srcBuffer, VulkanBuffer *_dstBuffer, VkDeviceSize _size)
    {
        // Create the command buffer
        VkCommandBufferAllocateInfo allocateInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = *_commandPool;

        allocateInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(*_device, &allocateInfo, &commandBuffer);

        // Record the command buffer
        VkCommandBufferBeginInfo commandBufferBeginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

        VkBufferCopy copyRegion{
            .srcOffset = 0,
            .dstOffset = 0,
            .size = _size};

        vkCmdCopyBuffer(commandBuffer, _srcBuffer->Buffer, _dstBuffer->Buffer, 1, &copyRegion);
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(*_queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(*_queue);

        vkFreeCommandBuffers(*_device, *_commandPool, 1, &commandBuffer);
    }

    void VulkanBuffer::CopyBufferToImage(VkCommandBuffer _cmd, VkImageLayout _dstImageLayout, VulkanBuffer *_srcBuffer, VkImage *_dstImage, VkExtent3D _imageExtent)
    {
        VkBufferImageCopy copyRegion = {};
        copyRegion.bufferOffset = 0;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;

        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.mipLevel = 0;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageExtent = _imageExtent;

        vkCmdCopyBufferToImage(_cmd, _srcBuffer->Buffer, *_dstImage, _dstImageLayout, 1, &copyRegion);
    }

    /*
     * VERTEX BUFFER
     */
    VulkanVertexBuffer::VulkanVertexBuffer(VulkanRenderer *_renderer) : m_renderer(_renderer), m_bufferSize{0} {}

    VulkanVertexBuffer::~VulkanVertexBuffer()
    {
        m_buffer.reset();
        m_buffer = nullptr;
    }

    void VulkanVertexBuffer::UploadData(const std::vector<Vertex> &_vertices)
    {
        uint64_t newBufferSize{_vertices.size() * sizeof(Vertex)};

        // If the buffer size changed, we need to recreate it
        if (m_bufferSize != newBufferSize)
        {
            // if the buffer exists, reset it (clear old buffer)
            if (m_buffer)
                m_buffer.reset();

            m_bufferSize = newBufferSize;
            m_count = static_cast<uint64_t>(_vertices.size());

            m_buffer = std::make_shared<VulkanBuffer>(
                &m_renderer->m_allocator, m_bufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VMA_MEMORY_USAGE_GPU_ONLY);
        }

        auto stagingBuffer = std::make_shared<VulkanBuffer>(
            &m_renderer->m_allocator, m_bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VMA_MEMORY_USAGE_CPU_ONLY);
        stagingBuffer->UploadData((int *)_vertices.data(), m_bufferSize);

        auto &frame = m_renderer->GetCurrentFrame();
        VulkanBuffer::CopyBuffer(&m_renderer->m_device, &frame.CommandPool, &m_renderer->m_graphicsQueue,
                                 stagingBuffer.get(), m_buffer.get(), m_bufferSize);
    }

    void VulkanVertexBuffer::Bind()
    {
        if (m_buffer)
        {
            VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(m_renderer->GetCurrentFrame().MainCommandBuffer, 0, 1, &m_buffer->Buffer, &offset);
        }
    }

    /*
     * INDEX BUFFER
     */

    VulkanIndexBuffer::VulkanIndexBuffer(VulkanRenderer *_renderer) : m_renderer{_renderer}, m_bufferSize{0} {}

    VulkanIndexBuffer::~VulkanIndexBuffer()
    {
        m_buffer.reset();
        m_buffer = nullptr;
    }

    void VulkanIndexBuffer::Bind()
    {
        if (m_buffer)
        {
            VkDeviceSize offset = 0;
            vkCmdBindIndexBuffer(m_renderer->GetCurrentFrame().MainCommandBuffer, m_buffer->Buffer, offset, VK_INDEX_TYPE_UINT32);
        }
    }

    void VulkanIndexBuffer::UploadData(const std::vector<uint32_t> &_indices)
    {
        uint64_t newBufferSize{_indices.size() * sizeof(uint32_t)};

        // If the buffer size changed, we need to recreate it
        if (m_bufferSize != newBufferSize)
        {
            // if the buffer exists, reset it (clear old buffer)
            if (m_buffer)
                m_buffer.reset();

            m_bufferSize = newBufferSize;
            m_count = static_cast<uint64_t>(_indices.size());
            m_buffer = std::make_shared<VulkanBuffer>(
                &m_renderer->m_allocator, m_bufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VMA_MEMORY_USAGE_GPU_ONLY);
        }

        auto stagingBuffer = std::make_shared<VulkanBuffer>(
            &m_renderer->m_allocator, m_bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VMA_MEMORY_USAGE_CPU_ONLY);
        stagingBuffer->UploadData((int *)_indices.data(), m_bufferSize);

        auto &frame = m_renderer->GetCurrentFrame();
        VulkanBuffer::CopyBuffer(&m_renderer->m_device, &frame.CommandPool, &m_renderer->m_graphicsQueue,
                                 stagingBuffer.get(), m_buffer.get(), m_bufferSize);
    }

    /*
     * UNIFORM BUFFER
     */
    VulkanUniformBuffer::VulkanUniformBuffer(VulkanRenderer *_renderer) : m_renderer{_renderer}, m_bufferSize{0} {}

    VulkanUniformBuffer::~VulkanUniformBuffer()
    {
        m_buffer.reset();
        m_buffer = nullptr;
    }

    VkDescriptorSet VulkanUniformBuffer::GetDescriptorSet(VkDescriptorSetLayout *_descriptorSetLayout)
    {
        if (!m_descriptorSet)
        {
            if (!m_buffer)
            {
                // Give it default data if no data was explicitely given yet
                UploadData({});
            }

            VkDescriptorSetAllocateInfo allocateInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
            allocateInfo.descriptorPool = m_renderer->m_descriptorPool;
            allocateInfo.descriptorSetCount = 1;
            allocateInfo.pSetLayouts = _descriptorSetLayout;

            VK_CHECK(vkAllocateDescriptorSets(m_renderer->m_device, &allocateInfo, &m_descriptorSet));

            if (m_buffer)
            {
                VkDescriptorBufferInfo descriptorBufferInfo{};
                descriptorBufferInfo.buffer = m_buffer->Buffer;
                descriptorBufferInfo.offset = 0;
                descriptorBufferInfo.range = sizeof(UniformBufferObject);

                VkWriteDescriptorSet descriptorSetWrite{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
                descriptorSetWrite.dstSet = m_descriptorSet;
                descriptorSetWrite.dstBinding = 0;
                descriptorSetWrite.dstArrayElement = 0;
                descriptorSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorSetWrite.descriptorCount = 1;
                descriptorSetWrite.pBufferInfo = &descriptorBufferInfo;
                descriptorSetWrite.pImageInfo = nullptr;
                descriptorSetWrite.pTexelBufferView = nullptr;

                vkUpdateDescriptorSets(m_renderer->m_device, 1, &descriptorSetWrite, 0, nullptr);
            }
        }
        return m_descriptorSet;
    }

    void VulkanUniformBuffer::Bind() {}

    void VulkanUniformBuffer::UploadData(const UniformBufferObject &_object)
    {
        uint64_t newBufferSize{sizeof(_object)};

        // If the buffer size changed, we need to recreate it
        if (m_bufferSize != newBufferSize)
        {
            // if the buffer exists, reset it (clear old buffer)
            if (m_buffer)
                m_buffer.reset();

            m_bufferSize = newBufferSize;
            m_buffer = std::make_shared<VulkanBuffer>(
                &m_renderer->m_allocator, m_bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VMA_MEMORY_USAGE_CPU_TO_GPU);
        }
        m_buffer->UploadData((int *)&_object, m_bufferSize);
    }

    void VulkanUniformBuffer::ResetDescriptorSet()
    {
        m_descriptorSet = VK_NULL_HANDLE;
    }
}
