#pragma once
#include "engine.h"
#include "buffers/command_buffer.h"
#include "buffers/command_pool.h"
#include "devices/instance.h"
#include "devices/logical_device.h"
#include "devices/physical_device.h"
#include "devices/surface.h"
#include "vulkan_renderer.h"
#include <map>

namespace Avarice
{
    class Graphics
    {
    public:
        Graphics();
        ~Graphics();
        static Graphics *Get() { return m_Graphics; }
        static std::string StringifyResultVk(VkResult result);
        static void CheckVk(VkResult result);
        void CaptureScreenshot(const std::filesystem::path &filename) const;
        const std::shared_ptr<CommandPool> &GetCommandPool(const std::thread::id &threadId = std::this_thread::get_id());
        Renderer *GetRenderer() const { return renderer.get(); }
        void SetRenderer(std::unique_ptr<Renderer> &&renderer) { this->renderer = std::move(renderer); }
        const PhysicalDevice *GetPhysicalDevice() const { return physicalDevice.get(); }
        const Surface *GetSurface() const { return surface.get(); }
        const LogicalDevice *GetLogicalDevice() const { return logicalDevice.get(); }

        void Update();

    private:
        void CreatePipelineCache();
        void ResetRenderStages();
        void RecreateSwapchain();
        void RecreateCommandBuffers();
        void RecreateAttachmentsMap();
        //void RecreatePass(RenderStage &renderStage);
        //bool StartRenderpass(RenderStage &renderStage);
        //void EndRenderpass(RenderStage &renderStage);
        //std::map<std::string, const Descriptor *> attachments;
        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<Swapchain> swapchain;
        
        std::vector<VkSemaphore> presentCompletes;
        std::vector<VkSemaphore> renderCompletes;
        std::vector<VkFence> flightFences;
        std::size_t currentFrame = 0;
        bool framebufferResized = false;

        std::map<std::thread::id, std::shared_ptr<CommandPool>> commandPools;
        std::vector<std::unique_ptr<CommandBuffer>> commandBuffers;

        VkPipelineCache pipelineCache = VK_NULL_HANDLE;
        ElapsedTime elapsedPurge;
        std::unique_ptr<Instance> instance;
        std::unique_ptr<PhysicalDevice> physicalDevice;
        std::unique_ptr<Surface> surface;
        std::unique_ptr<LogicalDevice> logicalDevice;

    protected:
        inline static Graphics *m_Graphics = nullptr;
    };
}
