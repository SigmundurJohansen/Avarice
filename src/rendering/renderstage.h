#pragma once

#include "renderpass/framebuffer.h"
#include "renderpass/renderpass.h"
#include "renderpass/swapchain.h"
#include "descriptors/descriptor.h"
#include "utils/vector2.h"
#include <map>
#include <string>
#include <memory>

namespace Avarice
{

    class Attachment
    {
    public:
        enum class Type
        {
            Image,
            Depth,
            Swapchain
        };
        
        Attachment(uint32_t _binding, std::string _name, Type _type, bool _multisampled = false) :  m_binding(_binding),
                                                                                                    m_name(std::move(_name)),
                                                                                                    m_type(_type),
                                                                                                    m_multisampled(_multisampled)
        {
        }

        uint32_t GetBinding() const { return m_binding; }
        const std::string &GetName() const { return m_name; }
        Type GetType() const { return m_type; }
        bool IsMultisampled() const { return m_multisampled; }

    private:
        uint32_t m_binding;
        std::string m_name;
        Type m_type;
        bool m_multisampled;
    };

    class SubpassType
    {
    public:
        SubpassType(uint32_t _binding, std::vector<uint32_t> _attachmentBindings) : m_binding(_binding),
                                                                                  m_attachmentBindings(std::move(_attachmentBindings))
        {
        }

        uint32_t GetBinding() const { return m_binding; }
        const std::vector<uint32_t> &GetAttachmentBindings() const { return m_attachmentBindings; }

    private:
        uint32_t m_binding;
        std::vector<uint32_t> m_attachmentBindings;
    };

    class RenderArea
    {
    public:
        explicit RenderArea(const Vector2ui &_extent = {}, const Vector2i &_offset = {}) : m_extent(_extent),
                                                                                         m_offset(_offset)
        {
        }

        bool operator==(const RenderArea &rhs) const
        {
            return m_extent == rhs.m_extent && m_offset == rhs.m_offset;
        }

        bool operator!=(const RenderArea &rhs) const
        {
            return !operator==(rhs);
        }

        const Vector2ui &GetExtent() const { return m_extent; }
        void SetExtent(const Vector2ui &_extent) { this->m_extent = _extent; }

        const Vector2i &GetOffset() const { return m_offset; }
        void SetOffset(const Vector2i &_offset) { this->m_offset = _offset; }

        /**
	 * Gets the aspect ratio between the render stages width and height.
	 * @return The aspect ratio.
	 */
        float GetAspectRatio() const { return m_aspectRatio; }
        void SetAspectRatio(float _aspectRatio) { this->m_aspectRatio = _aspectRatio; }

    private:
        Vector2ui m_extent;
        Vector2i m_offset;
        float m_aspectRatio = 1.0f;
    };

    class Viewport
    {
    public:
        Viewport() = default;

        explicit Viewport(const Vector2ui &_size) : m_size(_size)
        {
        }

        const Vector2f &GetScale() const { return m_scale; }
        void SetScale(const Vector2f &_scale) { this->m_scale = _scale; }

        const std::optional<Vector2ui> &GetSize() const { return m_size; }
        void SetSize(const std::optional<Vector2ui> &_size) { this->m_size = _size; }

        const Vector2i &GetOffset() const { return m_offset; }
        void SetOffset(const Vector2i &_offset) { this->m_offset = _offset; }

    private:
        Vector2f m_scale = {1.0f, 1.0f};
        std::optional<Vector2ui> m_size;
        Vector2i m_offset;
    };

    class RenderStage
    {
        friend class Graphics;

    public:
        explicit RenderStage(std::vector<Attachment> _images = {}, std::vector<SubpassType> _subpasses = {}, const Viewport &_viewport = Viewport());

        void Update();
        void Rebuild(const Swapchain &_swapchain);

        std::optional<Attachment> GetAttachment(const std::string &_name) const;
        std::optional<Attachment> GetAttachment(uint32_t _binding) const;

        const Descriptor *GetDescriptor(const std::string &_name) const;
        const VkFramebuffer &GetActiveFramebuffer(uint32_t _activeSwapchainImage) const;

        const std::vector<Attachment> &GetAttachments() const { return m_attachments; }
        const std::vector<SubpassType> &GetSubpasses() const { return m_subpasses; }

        Viewport &GetViewport() { return m_viewport; }
        void SetViewport(const Viewport &_viewport) { this->m_viewport = _viewport; }

        /**
	 * Gets the render stage viewport.
	 * @return The the render stage viewport.
	 */
        const RenderArea &GetRenderArea() const { return m_renderArea; }

        /**
	 * Gets if the width or height has changed between the last update and now.
	 * @return If the width or height has changed.
	 */
        bool IsOutOfDate() const { return m_outOfDate; }

        const Renderpass *GetRenderpass() const { return m_renderpass.get(); }
        //const ImageDepth *GetDepthStencil() const { return depthStencil.get(); }
        const Framebuffers *GetFramebuffers() const { return m_framebuffers.get(); }
        const std::vector<VkClearValue> &GetClearValues() const { return m_clearValues; }
        uint32_t GetAttachmentCount(uint32_t _subpass) const { return m_subpassAttachmentCount[_subpass]; }
        bool HasDepth() const { return m_depthAttachment.has_value(); }
        bool HasSwapchain() const { return m_swapchainAttachment.has_value(); }
        bool IsMultisampled(uint32_t subpass) const { return m_subpassMultisampled[subpass]; }

    private:
        std::vector<Attachment> m_attachments;
        std::vector<SubpassType> m_subpasses;

        Viewport m_viewport;

        std::unique_ptr<Framebuffers> m_framebuffers;
        std::unique_ptr<Renderpass> m_renderpass;

        //std::unique_ptr<ImageDepth> depthStencil;
        std::map<std::string, const Descriptor* > m_descriptors;

        std::vector<VkClearValue> m_clearValues;
        std::vector<uint32_t> m_subpassAttachmentCount;
        std::optional<Attachment> m_depthAttachment;
        std::optional<Attachment> m_swapchainAttachment;
        std::vector<bool> m_subpassMultisampled;

        RenderArea m_renderArea;
        bool m_outOfDate = false;
    };
}
