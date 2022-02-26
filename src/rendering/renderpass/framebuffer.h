#pragma once
#include <vulkan/vulkan.h>
#include "swapchain.h"
#include "utils/non_copyable.h"
#include <vector>
#include "utils/vector2.h"

namespace Avarice
{
    class Renderpass;
    class RenderStage;

    class Framebuffers : NonCopyable
    {
    public:
        Framebuffers(const Vector2ui &extent, const RenderStage &renderStage, const Renderpass &renderPass, const Swapchain &swapchain,
                     VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
        ~Framebuffers();

        //Image2d *GetAttachment(uint32_t index) const { return imageAttachments[index].get(); }

        //const std::vector<std::unique_ptr<Image2d>> &GetImageAttachments() const { return imageAttachments; }
        const std::vector<VkFramebuffer> &GetFramebuffers() const { return framebuffers; }

    private:
        //std::vector<std::unique_ptr<Image2d>> imageAttachments;
        std::vector<VkFramebuffer> framebuffers;
    };
}
