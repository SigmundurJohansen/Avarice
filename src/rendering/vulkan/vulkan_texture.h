#pragma once

#include <../rendering/texture.h>
#include "vulkan_renderer.h"

namespace Avarice
{
    class VulkanTexture : public Texture
    {
    public:
        explicit VulkanTexture(VulkanRenderer *_renderer);
        ~VulkanTexture() override;

        void WriteToDescriptorSet(VkDescriptorSet _descriptorSet, int _dstBinding);
        void ResetDescriptorSet();

        void Bind() override;
        void BindSamplerSettings() override;

        void UploadData(const ImageData &_data) override;

        [[nodiscard]] std::pair<uint32_t, uint32_t> GetSize() const override;

        [[nodiscard]] int *GetHandle() const override;

    private:
        VulkanRenderer *m_renderer{nullptr};

        uint32_t m_width{0};
        uint32_t m_height{0};

        VkImage m_image{VK_NULL_HANDLE};
        VmaAllocation m_allocation{VK_NULL_HANDLE};
        VkImageView m_imageView{VK_NULL_HANDLE};
        VkSampler m_sampler{VK_NULL_HANDLE};
    };
}
