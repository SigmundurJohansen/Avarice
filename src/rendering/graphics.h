#pragma once
#include "buffers/command_buffer.h"
#include "buffers/command_pool.h"
#include "devices/instance.h"
#include "devices/logical_device.h"
#include "devices/physical_device.h"
#include "devices/surface.h"
#include "renderpass/swapchain.h"
#include "renderpass/renderpass.h"
#include "vulkan_renderer.h"
#include "descriptors/descriptor.h"
#include "renderstage.h"
#include "rendermanager.h"
#include <map>
#include <string>

namespace Avarice
{
    class Graphics
    {
    public:
        Graphics();
        ~Graphics();
        static Graphics *Get() { return m_Graphics; }
        static std::string StringifyResultVk(VkResult _result);
        static void CheckVk(VkResult _result);
        const std::shared_ptr<CommandPool> &GetCommandPool(const std::thread::id &_threadId = std::this_thread::get_id());
        RenderManager *GetRenderManager() const { return m_renderManager.get(); }
        void SetRenderer(std::unique_ptr<RenderManager> &&_renderManager) { this->m_renderManager = std::move(_renderManager); }
        const PhysicalDevice *GetPhysicalDevice() const { return m_physicalDevice.get(); }
        const Surface *GetSurface() const { return m_surface.get(); }
        const LogicalDevice *GetLogicalDevice() const { return m_logicalDevice.get(); }

        void Update();

    private:
        void CreatePipelineCache();
        void ResetRenderStages();
        void RecreateSwapchain();
        void RecreateCommandBuffers();
        void RecreateAttachmentsMap();
        void RecreatePass(RenderStage &_renderStage);
        bool StartRenderpass(RenderStage &_renderStage);
        void EndRenderpass(RenderStage &_renderStage);
        std::map<std::string, const Descriptor *> m_attachments;
        std::unique_ptr<RenderManager> m_renderManager;
        std::unique_ptr<Swapchain> m_swapchain;

        std::vector<VkSemaphore> m_presentCompletes;
        std::vector<VkSemaphore> m_renderCompletes;
        std::vector<VkFence> m_flightFences;
        std::size_t m_currentFrame = 0;
        bool m_framebufferResized = false;

        std::map<std::thread::id, std::shared_ptr<CommandPool>> m_commandPools;
        std::vector<std::unique_ptr<CommandBuffer>> m_commandBuffers;

        VkPipelineCache m_pipelineCache = VK_NULL_HANDLE;
        ElapsedTime m_elapsedPurge;
        std::unique_ptr<Instance> m_instance;
        std::unique_ptr<PhysicalDevice> m_physicalDevice;
        std::unique_ptr<Surface> m_surface;
        std::unique_ptr<LogicalDevice> m_logicalDevice;

    protected:
        inline static Graphics *m_Graphics = nullptr;
    };
}
