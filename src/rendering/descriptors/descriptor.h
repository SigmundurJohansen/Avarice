#pragma once

#include <cstdint>
#include <optional>
#include <memory>

namespace Avarice
{
	class OffsetSize
	{
	public:
		OffsetSize(uint32_t offset, uint32_t size) : offset(offset),
													 size(size)
		{
		}

		uint32_t GetOffset() const { return offset; }
		uint32_t GetSize() const { return size; }

		bool operator==(const OffsetSize &rhs) const
		{
			return offset == rhs.offset && size == rhs.size;
		}

		bool operator!=(const OffsetSize &rhs) const
		{
			return !operator==(rhs);
		}

	private:
		uint32_t offset;
		uint32_t size;
	};

	class WriteDescriptorSet
	{
	public:
		WriteDescriptorSet(	const VkWriteDescriptorSet &_writeDescriptorSet, 
							const VkDescriptorImageInfo &_imageInfo) : m_writeDescriptorSet(_writeDescriptorSet),
							m_imageInfo(std::make_unique<VkDescriptorImageInfo>(_imageInfo))
		{
			this->m_writeDescriptorSet.pImageInfo = this->m_imageInfo.get();
		}

		WriteDescriptorSet(	const VkWriteDescriptorSet &_writeDescriptorSet, 
							const VkDescriptorBufferInfo &_bufferInfo) : m_writeDescriptorSet(_writeDescriptorSet),
							m_bufferInfo(std::make_unique<VkDescriptorBufferInfo>(_bufferInfo))
		{
			this->m_writeDescriptorSet.pBufferInfo = this->m_bufferInfo.get();
		}

		const VkWriteDescriptorSet &GetWriteDescriptorSet() const { return m_writeDescriptorSet; }

	private:
		VkWriteDescriptorSet m_writeDescriptorSet;
		std::unique_ptr<VkDescriptorImageInfo> m_imageInfo;
		std::unique_ptr<VkDescriptorBufferInfo> m_bufferInfo;
	};

	class Descriptor
	{
	public:
		Descriptor() = default;

		virtual ~Descriptor() = default;

		virtual WriteDescriptorSet GetWriteDescriptor(uint32_t _binding, VkDescriptorType _descriptorType, const std::optional<OffsetSize> &_offsetSize) const = 0;
	};
}
