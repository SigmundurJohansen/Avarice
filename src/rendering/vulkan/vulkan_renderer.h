#pragma once

#include <vulkan/vulkan.h>
#include <../external/vk-bootstrap/src/VkBootstrap.h>
#include "../renderer.h"
#include "vulkan_initializers.h"
#include "vulkan_types.h"
#include "vulkan_utilities.h"
#include <vector>

namespace Avarice
{
class VulkanRenderer : public Renderer{
    public:
    void Init(RendererSettings _settings) override;
    void Shutdown() override;
    void RenderFrame() override;

    private:
    // todo: temporary frame number
    uint64_t m_frameNumber {0};

    RendererSettings m_rendererSettings{};
    //core vulkan
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;
    VkSurfaceKHR m_surface;
    // swapchain
    VkSwapchainKHR m_swapchain;
    VkFormat m_swapchainImageFormat;
    std::vector<VkImage> m_swapchainImages;
    std::vector<VkImageView> m_swapchainImageViews;
    VkExtent2D m_windowExtent;
    // command pools and queues
    VkQueue m_graphicsQueue;
    uint32_t m_graphicsQueueFamily;
    VkCommandPool m_commandPool;
    VkCommandBuffer m_mainCommandBuffer;
    // Render passes
    VkRenderPass m_renderPass;
    std::vector<VkFramebuffer> m_framebuffers{3};
    // Syncronization objects
    VkSemaphore m_presentSemaphore, m_renderSemaphore;
    VkFence m_renderFence;

    void InitCore();
    void CreateSwapchain();
    void CreateCommands();
    void CreateDefaultRenderPass();
    void CreateFramebuffers();
    void CreateSyncStructures();
};
}