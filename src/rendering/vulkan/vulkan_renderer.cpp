#include "vulkan_renderer.h"
#include "service_locator.h"

namespace Avarice
{
    void VulkanRenderer::Init(RendererSettings _settings) 
    {
        m_rendererSettings = _settings;
        InitCore();
        CreateSwapchain();
        CreateCommands();
        CreateDefaultRenderPass();
        CreateFramebuffers();
        CreateSyncStructures();
    }

    void VulkanRenderer::Shutdown()
    {
        for(auto framebuffer : m_framebuffers)
        {
            vkDestroyFramebuffer(m_device,framebuffer, nullptr);
        }
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);

        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

        for(auto imageView :m_swapchainImageViews)
        {
            vkDestroyImageView(m_device, imageView, nullptr);
        }

        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger);
        vkDestroyInstance(m_instance,nullptr);
    }
    void VulkanRenderer::RenderFrame()
    {
        VK_CHECK(vkWaitForFences(m_device, 1, &m_renderFence, true, 1000000000)); //1
        VK_CHECK(vkResetFences(m_device,1,&m_renderFence)); //0

        uint32_t swapchainIndex;
        VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapchain, 1000000000, m_presentSemaphore, nullptr, &swapchainIndex));
        VK_CHECK(vkResetCommandBuffer(m_mainCommandBuffer, 0));
        VkCommandBuffer cmd = m_mainCommandBuffer;
        VkCommandBufferBeginInfo beginInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

        float flashColour = abs(sin((float)m_frameNumber/120.f));
        VkClearValue clearValue{
            .color = {0.f, 0.f, flashColour, 1.f}
        };

        VkRenderPassBeginInfo renderPassBeginInfo { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
        renderPassBeginInfo.renderPass = m_renderPass;
        renderPassBeginInfo.renderArea = {
            .offset = {
                .x = 0,
                .y = 0
            },
            .extent = m_windowExtent
        };

        renderPassBeginInfo.framebuffer = m_framebuffers[swapchainIndex];
        // connect clear values
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;

        vkCmdBeginRenderPass(cmd, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        // draw calls
        vkCmdEndRenderPass(cmd);
        VK_CHECK(vkEndCommandBuffer(cmd));

        VkSubmitInfo submit { VK_STRUCTURE_TYPE_SUBMIT_INFO};
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit.pWaitDstStageMask = &waitStage;

        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &m_presentSemaphore;

        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &m_renderSemaphore;
        
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &m_mainCommandBuffer;

        VK_CHECK(vkQueueSubmit(m_graphicsQueue, 1, &submit, m_renderFence));

        VkPresentInfoKHR presentInfoKhr { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
        presentInfoKhr.swapchainCount = 1;
        presentInfoKhr.pSwapchains = &m_swapchain;

        presentInfoKhr.waitSemaphoreCount = 1;
        presentInfoKhr.pWaitSemaphores = &m_renderSemaphore;

        presentInfoKhr.pImageIndices = &swapchainIndex;
        VK_CHECK(vkQueuePresentKHR(m_graphicsQueue, &presentInfoKhr));
        m_frameNumber++;
    }
    void VulkanRenderer::InitCore()
    {
        vkb::InstanceBuilder builder;
        auto builderInstance = builder.set_app_name(m_rendererSettings.ApplicationName.c_str())
                                                    .request_validation_layers(true)
                                                    .require_api_version(1,1,0)
                                                    .use_default_debug_messenger()
                                                    .build();
        vkb::Instance vkb_inst = builderInstance.value();

        m_instance = vkb_inst.instance;
        m_debugMessenger = vkb_inst.debug_messenger;
        
        // request vulkan surface
        std::unordered_map<SurfaceArgs, std::any> surfaceArgs{
            {SurfaceArgs::INSTANCE, m_instance},
            {SurfaceArgs::ALLOCATORS, nullptr},
            {SurfaceArgs::OUT_SURFACE, &m_surface}
        };

        ServiceLocator::GetWindow()->RequestDrawSurface(surfaceArgs);

        // select physical devcice
        vkb::PhysicalDeviceSelector selector {vkb_inst};
        vkb::PhysicalDevice vkbPhysicalDevice = selector .set_minimum_version(1,1) .set_surface(m_surface) .select() .value();

        // create the vulkan device
        vkb::DeviceBuilder deviceBuilder {vkbPhysicalDevice};
        vkb::Device vkbDevice { deviceBuilder.build().value() };

        m_device = vkbDevice.device;
        m_physicalDevice = vkbPhysicalDevice.physical_device;

        m_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        m_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
    }

    void VulkanRenderer::CreateSwapchain()
    {
        auto [width, height] = ServiceLocator::GetWindow()->GetWindowExtents();
        m_windowExtent.height = height;
        m_windowExtent.width = width;
        vkb::SwapchainBuilder swapchainBuilder { m_physicalDevice, m_device, m_surface};
        vkb::Swapchain vkbSwapchain = swapchainBuilder 
                                        .use_default_format_selection() 
                                        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) //hard vsync 
                                        .set_desired_extent(width, height)
                                        .build()
                                        .value();
        
        // store swapchain and all related images
        m_swapchain = vkbSwapchain.swapchain;
        m_swapchainImages = vkbSwapchain.get_images().value();
        m_swapchainImageViews = vkbSwapchain.get_image_views().value();
        m_swapchainImageFormat = vkbSwapchain.image_format;
    }
    void VulkanRenderer::CreateCommands()
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo = VulkanInitializers::CommandPoolCreateInfo(m_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);        
        VK_CHECK(vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_commandPool ) );

        VkCommandBufferAllocateInfo commandBufferAllocateInfo = VulkanInitializers::CommandBufferAllocateInfo(m_commandPool);
        VK_CHECK(vkAllocateCommandBuffers(m_device, &commandBufferAllocateInfo, &m_mainCommandBuffer));

    }
    
    void VulkanRenderer::CreateDefaultRenderPass()
    {
        VkAttachmentDescription colorAttachment{
            .format = m_swapchainImageFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };

        VkAttachmentReference colorAttachmentRef{
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        VkSubpassDescription subpass{
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = & colorAttachmentRef
        };


        VkRenderPassCreateInfo renderPassCreateInfo { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &colorAttachment;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        VK_CHECK(vkCreateRenderPass(m_device, &renderPassCreateInfo, nullptr, &m_renderPass));
    }

    void VulkanRenderer::CreateFramebuffers()
    {
        VkFramebufferCreateInfo framebufferCreateInfo { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        framebufferCreateInfo.renderPass = m_renderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.width = m_windowExtent.width;
        framebufferCreateInfo.height = m_windowExtent.height;
        framebufferCreateInfo.layers = 1;

        const uint32_t swapchainImageCount = m_swapchainImages.size();
        m_framebuffers.resize(swapchainImageCount); 

        for(int i = 0; i < swapchainImageCount; i ++)
        {
            framebufferCreateInfo.pAttachments = &m_swapchainImageViews[i];
            VK_CHECK(vkCreateFramebuffer(m_device,&framebufferCreateInfo,nullptr,&m_framebuffers[i]));
        }
    }
    void VulkanRenderer::CreateSyncStructures()
    {
        VkFenceCreateInfo fenceCreateINfo { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        fenceCreateINfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VK_CHECK(vkCreateFence(m_device, &fenceCreateINfo, nullptr, &m_renderFence));

        VkSemaphoreCreateInfo semaphoreCreateInfo { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_presentSemaphore));
        VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_renderSemaphore));
    }
}