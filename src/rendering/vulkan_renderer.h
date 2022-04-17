#pragma once
#include <../rendering/renderer.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <optional>
#include <set>

namespace Avarice
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> m_graphicsFamily;
        std::optional<uint32_t> m_presentFamily;

        bool isComplete()
        {
            return m_graphicsFamily.has_value() && m_presentFamily.has_value();
        }
    };
    class VulkanRenderer : public Renderer
    {
        friend class VulkanShader;

    private:
        static VulkanRenderer m_renderer;
        const std::vector<const char *> m_deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    public:
        VulkanRenderer &GetGO() { return m_renderer; }
        void Init(RendererSettings _settings) override;
        void Shutdown() override;
        void RenderFrame() override;
        std::shared_ptr<Shader> CreateShader() override;
        //private:
        // todo: temporary frame number
        bool started = false;
        uint64_t m_frameNumber{0};

        RendererSettings m_rendererSettings{};
        //core vulkan
        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_debugMessenger;
        VkPhysicalDevice m_physicalDevice; // gpu
        VkDevice m_device;                 //cpu/logic device
        VkSurfaceKHR m_surface;
        // swapchain
        QueueFamilyIndices m_indices;
        VkSwapchainKHR m_swapchain;
        VkFormat m_swapchainImageFormat;
        std::vector<VkImage> m_swapchainImages;
        std::vector<VkImageView> m_swapchainImageViews;
        VkExtent2D m_windowExtent;
        // command pools and queues
        VkQueue m_presentQueue;
        VkQueue m_graphicsQueue;
        uint32_t m_graphicsQueueFamily;
        VkCommandPool m_commandPool;
        VkCommandBuffer m_mainCommandBuffer;

        VkDescriptorPool m_descriptorPool;
        // Render passes
        VkRenderPass m_renderPass;
        std::vector<VkFramebuffer> m_framebuffers{3};
        // Syncronization objects
        VkSemaphore m_presentSemaphore, m_renderSemaphore;
        VkFence m_renderFence;
        // Temporary runtime game objectws
        std::shared_ptr<Shader> m_triangleShader{};

        void InitCore();
        void CreateSwapchain();
        void CreateCommands();
        void CreateDefaultRenderPass();
        void CreateFramebuffers();
        void CreateSyncStructures();
        void CreatePipelines();
        void InitImgui();
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice _device);
        bool isDeviceSuitable(VkPhysicalDevice _device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice _device);
    };
}