#pragma once

#include <../rendering/renderer.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <fstream>

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
    // PIPELINES
    VkPipelineLayout m_trianglePipelineLayout;
    VkPipeline m_trianglePipeline;

    void InitCore();
    void CreateSwapchain();
    void CreateCommands();
    void CreateDefaultRenderPass();
    void CreateFramebuffers();
    void CreateSyncStructures();
    void CreatePipelines();
};
}