#include "vulkan_buffer.h"
#include "vulkan_renderer.h"
#include <memory>
#include <vector>
#include <cstring>
#include "vulkan_utilities.h"
#include <vector>

namespace Avarice
{

  VkDeviceSize VulkanBuffer::getAlignment(VkDeviceSize _instanceSize, VkDeviceSize _minOffsetAlignment)
  {
    if (_minOffsetAlignment > 0)
    {
      return (_instanceSize + _minOffsetAlignment - 1) & ~(_minOffsetAlignment - 1);
    }
    return _instanceSize;
  }

  VulkanBuffer::VulkanBuffer(
      VulkanDevice &_device,
      VkDeviceSize _instanceSize,
      uint32_t _instanceCount,
      VkBufferUsageFlags _usageFlags,
      VkMemoryPropertyFlags _memoryPropertyFlags,
      VkDeviceSize _minOffsetAlignment)
      : m_device{_device},
        m_instanceSize{_instanceSize},
        m_instanceCount{_instanceCount},
        m_usageFlags{_usageFlags},
        m_memoryPropertyFlags{_memoryPropertyFlags}
  {
    m_alignmentSize = getAlignment(_instanceSize, _minOffsetAlignment);
    m_bufferSize = m_alignmentSize * _instanceCount;
    m_device.createBuffer(m_bufferSize, m_usageFlags, m_memoryPropertyFlags, m_buffer, m_memory);
  }

  VulkanBuffer::~VulkanBuffer()
  {
    unmap();
    vkDestroyBuffer(m_device.device(), m_buffer, nullptr);
    vkFreeMemory(m_device.device(), m_memory, nullptr);
  }

  VkResult VulkanBuffer::map(VkDeviceSize size, VkDeviceSize offset)
  {
    assert(m_buffer && m_memory && "Called map on buffer before create");
    return vkMapMemory(m_device.device(), m_memory, offset, size, 0, &m_mapped);
  }

  void VulkanBuffer::unmap()
  {
    if (m_mapped)
    {
      vkUnmapMemory(m_device.device(), m_memory);
      m_mapped = nullptr;
    }
  }

  void VulkanBuffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset)
  {
    assert(m_mapped && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE)
    {
      memcpy(m_mapped, data, m_bufferSize);
    }
    else
    {
      char *memOffset = (char *)m_mapped;
      memOffset += offset;
      memcpy(memOffset, data, size);
    }
  }

  VkResult VulkanBuffer::flush(VkDeviceSize size, VkDeviceSize offset)
  {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = m_memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(m_device.device(), 1, &mappedRange);
  }

  VkResult VulkanBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
  {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = m_memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(m_device.device(), 1, &mappedRange);
  }

  VkDescriptorBufferInfo VulkanBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset)
  {
    return VkDescriptorBufferInfo{
        m_buffer,
        offset,
        size
    };
  }

  void VulkanBuffer::writeToIndex(void *data, int index)
  {
    writeToBuffer(data, m_instanceSize, index * m_alignmentSize);
  }

  VkResult VulkanBuffer::flushIndex(int index) { return flush(m_alignmentSize, index * m_alignmentSize); }

  VkDescriptorBufferInfo VulkanBuffer::descriptorInfoForIndex(int index)
  {
    return descriptorInfo(m_alignmentSize, index * m_alignmentSize);
  }

  VkResult VulkanBuffer::invalidateIndex(int index)
  {
    return invalidate(m_alignmentSize, index * m_alignmentSize);
  }
}
