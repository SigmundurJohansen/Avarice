#pragma once

#include <../external/vk-bootstrap/src/VkBootstrap.h>
#include "../../rendering/renderer.h"
#include "vulkan_initializers.h"
#include "vulkan_types.h"
#include "vulkan_utilities.h"

namespace Avarice
{
class VulkanRenderer : public Renderer{
    public:
    virtual void Init(RendererSettings _settings) override;
    virtual void Shutdown() override;
    virtual void RenderFrame() override;

    private:
    RendererSettings mRendererSettings{};
    //core vulkan
    VkInstance mInstance;
    VkDebugUtilsMessengerEXT mDebugMessenger;
    VkPhysicalDevice mPhysicalDevice;
    VkDevice mDevice;
    VkSurfaceKHR mSurface;

    void InitCore();
};
}