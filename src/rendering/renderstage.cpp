#include "renderstage.h"
#include "graphics.h"
//#include "Devices/Window.hpp"

namespace Avarice
{
    RenderStage::RenderStage(std::vector<Attachment> _images,
                             std::vector<SubpassType> _subpasses,
                             const Viewport &_viewport) : m_attachments(std::move(_images)),
                                                          m_subpasses(std::move(_subpasses)),
                                                          m_viewport(_viewport),
                                                          m_subpassAttachmentCount(this->m_subpasses.size()),
                                                          m_subpassMultisampled(this->m_subpasses.size())
    {
        for (const auto &image : m_attachments)
        {
            VkClearValue clearValue = {};
            switch (image.GetType())
            {
            case Attachment::Type::Image:

                for (const auto &subpass : this->m_subpasses)
                {
                    if (auto subpassBindings = subpass.GetAttachmentBindings();
                        std::find(subpassBindings.begin(), subpassBindings.end(), image.GetBinding()) != subpassBindings.end())
                    {
                        m_subpassAttachmentCount[subpass.GetBinding()]++;
                        if (image.IsMultisampled())
                            m_subpassMultisampled[subpass.GetBinding()] = true;
                    }
                }

                break;
            case Attachment::Type::Depth:
                clearValue.depthStencil = {1.0f, 0};
                m_depthAttachment = image;
                break;
            case Attachment::Type::Swapchain:
                m_swapchainAttachment = image;
                break;
            }

            m_clearValues.emplace_back(clearValue);
        }
    }

    void RenderStage::Update()
    {
        auto lastRenderArea = m_renderArea;
        Vector2<uint32_t> window = {500, 500};
        m_renderArea.SetOffset(m_viewport.GetOffset());

        if (m_viewport.GetSize())
            m_renderArea.SetExtent(m_viewport.GetScale() * *m_viewport.GetSize());
        else
            m_renderArea.SetExtent(m_viewport.GetScale() * window);

        m_renderArea.SetAspectRatio(static_cast<float>(m_renderArea.GetExtent().x) / static_cast<float>(m_renderArea.GetExtent().y));
        m_renderArea.SetExtent(m_renderArea.GetExtent() + m_renderArea.GetOffset());

        m_outOfDate = m_renderArea != lastRenderArea;
    }

    void RenderStage::Rebuild(const Swapchain &swapchain)
    {
        Update();

        auto physicalDevice = Graphics::Get()->GetPhysicalDevice();
        auto surface = Graphics::Get()->GetSurface();

        auto msaaSamples = physicalDevice->GetMsaaSamples();

        m_framebuffers = std::make_unique<Framebuffers>(m_renderArea.GetExtent(), *this, *m_renderpass, swapchain, msaaSamples);
        m_outOfDate = false;

        m_descriptors.clear();
    }

    std::optional<Attachment> RenderStage::GetAttachment(const std::string &_name) const
    {
        auto it = std::find_if(m_attachments.begin(), m_attachments.end(), [_name](const Attachment &a)
                               { return a.GetName() == _name; });

        if (it != m_attachments.end())
            return *it;
        return std::nullopt;
    }

    std::optional<Attachment> RenderStage::GetAttachment(uint32_t _binding) const
    {
        auto it = std::find_if(m_attachments.begin(), m_attachments.end(), [_binding](const Attachment &a)
                               { return a.GetBinding() == _binding; });

        if (it != m_attachments.end())
            return *it;
        return std::nullopt;
    }

    const Descriptor *RenderStage::GetDescriptor(const std::string &_name) const
    {
        auto it = m_descriptors.find(_name);

        if (it != m_descriptors.end())
            return it->second;
        return nullptr;
    }

    const VkFramebuffer &RenderStage::GetActiveFramebuffer(uint32_t _activeSwapchainImage) const
    {
        if (_activeSwapchainImage > m_framebuffers->GetFramebuffers().size())
            return m_framebuffers->GetFramebuffers().at(0);

        return m_framebuffers->GetFramebuffers().at(_activeSwapchainImage);
    }
}
