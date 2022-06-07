#pragma once
#include "../rendering/renderer.h"
#include <cmath>
#include <../external/vk-bootstrap/src/VkBootstrap.h>
#include "service_locator.h"
#include "vulkan_initializers.h"
#include "vulkan_utilities.h"
#include "vulkan_shader.h"
#include "vulkan_buffer.h"
#include "vulkan_texture.h"
#include <iostream>
#include <sys/stat.h>
#include <cmath>
#include <array>

namespace Avarice
{
    constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    struct FrameData
    {
        VkSemaphore PresentSemaphore{VK_NULL_HANDLE};
        VkSemaphore RenderSemaphore{VK_NULL_HANDLE};

        VkCommandPool CommandPool{VK_NULL_HANDLE};
        VkCommandBuffer MainCommandBuffer{VK_NULL_HANDLE};

        VkFence RenderFence{VK_NULL_HANDLE};
        uint32_t SwapchainImageIndex{0};
    };

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
        friend struct VulkanBuffer;
        friend class VulkanVertexBuffer;
        friend class VulkanIndexBuffer;
        friend class VulkanUniformBuffer;
        friend class VulkanShader;
        friend class VulkanTexture;

    public:
        VulkanRenderer &GetGO() { return m_renderer; }
        void Init(RendererSettings _settings) override;
        void Shutdown() override;
        void BeginFrame() override;
        void EndFrame() override;

        void DrawIndexBuffer(IndexBuffer *_buffer) override;

        void WaitForIdle() override;

        std::shared_ptr<Shader> CreateShader() override;
        std::shared_ptr<VertexBuffer> CreateVertexBuffer() override;
        std::shared_ptr<IndexBuffer> CreateIndexBuffer() override;
        std::shared_ptr<UniformBuffer> CreateUniformBuffer() override;
        std::shared_ptr<Texture> CreateTexture() override;

    private:
        void InitCore();
        void CreateSwapchain();
        void CleanupSwapchain();
        void RecreateSwapchain();
        void RebuildShaders();

        void BuildSwapchain();
        void CreateCommands();
        void CreateDescriptorPools();
        void CreateDefaultRenderPass();
        void CreateFramebuffers();
        void CreateSyncStructures();

        FrameData &GetCurrentFrame();
        /*
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice _device);
        bool isDeviceSuitable(VkPhysicalDevice _device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice _device);
        */
    private:
        static VulkanRenderer m_renderer;

        uint64_t m_frameNumber{0};
        bool m_recreateFrameBuffer{false};

        RendererSettings m_rendererSettings{};

        // core vulkan
        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_debugMessenger;
        VkPhysicalDevice m_physicalDevice; // gpu
        VkDevice m_device;                 // cpu/logic device
        VkSurfaceKHR m_surface;
        VmaAllocator m_allocator;

        // swapchain
        // QueueFamilyIndices m_indices;
        VkSwapchainKHR m_swapchain;
        VkFormat m_swapchainImageFormat;
        std::vector<VkImage> m_swapchainImages;
        std::vector<VkImageView> m_swapchainImageViews;

        VkImageView m_depthImageView{VK_NULL_HANDLE};
        VkImage m_depthImage{VK_NULL_HANDLE};
        VmaAllocation m_depthImageAllocation{VK_NULL_HANDLE};
        VkFormat m_depthFormat;

        VkExtent2D m_windowExtent;

        // command pools and queues
        // VkQueue m_presentQueue;
        VkQueue m_graphicsQueue;
        uint32_t m_graphicsQueueFamily;
        // VkCommandPool m_commandPool;
        // VkCommandBuffer m_mainCommandBuffer;

        static constexpr std::array<VkDescriptorPoolSize, 2> POOL_SIZES{
            VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
            VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}};
        VkDescriptorPool m_descriptorPool;

        // Render passes
        VkRenderPass m_renderPass;
        std::vector<VkFramebuffer> m_framebuffers{3};

        // Syncronization objects
        FrameData m_frames[MAX_FRAMES_IN_FLIGHT];
        /*
        VkSemaphore m_presentSemaphore, m_renderSemaphore;
        VkFence m_renderFence;
        */
        // Temporary runtime game objectws
        // std::shared_ptr<Shader> m_triangleShader{};

        std::vector<std::weak_ptr<Shader>> m_shaders{};
    };
}