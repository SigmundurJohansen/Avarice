#pragma once

#include <vulkan/vulkan.h>
#include <../external/vk-bootstrap/src/VkBootstrap.h>
#include "../renderer.h"
#include "vulkan_initializers.h"
#include "vulkan_types.h"
#include "vulkan_utilities.h"

namespace Avarice
{
class VulkanRenderer : public Renderer{
    public:
    void Init(RendererSettings _settings) override;
    void Shutdown() override;
    void RenderFrame() override;

    private:
    RendererSettings m_RendererSettings{};
    //core vulkan
    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_Device;
    VkSurfaceKHR m_Surface;

    void InitCore();
};
}