#include "vulkan_renderer.h"

namespace Avarice
{
    void VulkanRenderer::Init(RendererSettings _settings)
    {
        m_rendererSettings = _settings;
        InitCore();
        CreateSwapchain();
        CreateCommands();
        CreateDescriptorPools();
        CreateDefaultRenderPass();
        CreateFramebuffers();
        CreateSyncStructures();
        // CreatePipelines();
        //  InitImgui();
    }

    void VulkanRenderer::Shutdown()
    {
        WaitForIdle();

        CleanupSwapchain();
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

        vmaDestroyAllocator(m_allocator);

        for (auto &frame : m_frames)
        {
            vkDestroySemaphore(m_device, frame.PresentSemaphore, nullptr);
            vkDestroySemaphore(m_device, frame.RenderSemaphore, nullptr);
            vkDestroyFence(m_device, frame.RenderFence, nullptr);

            vkDestroyCommandPool(m_device, frame.CommandPool, nullptr);
        }

        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger);
        vkDestroyInstance(m_instance, nullptr);
    }

    void VulkanRenderer::BeginFrame()
    {
        VK_CHECK(vkWaitForFences(m_device, 1, &GetCurrentFrame().RenderFence, true, 1000000000)); // 1
        VK_CHECK(vkResetFences(m_device, 1, &GetCurrentFrame().RenderFence));                     // 0

        VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, 1000000000, GetCurrentFrame().PresentSemaphore,
                                                VK_NULL_HANDLE, &GetCurrentFrame().SwapchainImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapchain();
            return;
        }

        VK_CHECK(vkResetCommandBuffer(GetCurrentFrame().MainCommandBuffer, 0));

        VkCommandBuffer cmd = GetCurrentFrame().MainCommandBuffer;

        VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

        float flashColour = abs(sin((float)m_frameNumber / 120.f));

        VkClearValue clearValue{
            .color = {0.f, flashColour, flashColour, 1.f}};

        VkClearValue depthClear{
            .depthStencil = {
                .depth = 1.f}};

        VkRenderPassBeginInfo renderPassBeginInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
        renderPassBeginInfo.renderPass = m_renderPass;
        renderPassBeginInfo.renderArea = {
            .offset = {
                .x = 0,
                .y = 0},
            .extent = m_windowExtent};

        VkClearValue clearValues[] = {clearValue, depthClear};
        renderPassBeginInfo.framebuffer = m_framebuffers[GetCurrentFrame().SwapchainImageIndex];
        // connect clear values
        renderPassBeginInfo.clearValueCount = 2;
        renderPassBeginInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(cmd, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanRenderer::EndFrame()
    {
        auto cmd = GetCurrentFrame().MainCommandBuffer;
        vkCmdEndRenderPass(cmd);
        VK_CHECK(vkEndCommandBuffer(cmd));

        VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};

        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit.pWaitDstStageMask = &waitStage;

        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &GetCurrentFrame().PresentSemaphore;

        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &GetCurrentFrame().RenderSemaphore;

        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &GetCurrentFrame().MainCommandBuffer;

        vkQueueSubmit(m_graphicsQueue, 1, &submit, GetCurrentFrame().RenderFence);

        VkPresentInfoKHR presentInfoKhr{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
        presentInfoKhr.swapchainCount = 1;
        presentInfoKhr.pSwapchains = &m_swapchain;

        presentInfoKhr.waitSemaphoreCount = 1;
        presentInfoKhr.pWaitSemaphores = &GetCurrentFrame().RenderSemaphore;

        presentInfoKhr.pImageIndices = &GetCurrentFrame().SwapchainImageIndex;

        auto result = vkQueuePresentKHR(m_graphicsQueue, &presentInfoKhr);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_recreateFrameBuffer)
        {
            m_recreateFrameBuffer = false;
            RecreateSwapchain();
        }
        else
        {
            VK_CHECK(result);
        }

        // Remove any expired shaders from the weak references
        erase_if(m_shaders, [](auto shader)
                 { return shader.expired(); });

        m_frameNumber++;
    }

    void VulkanRenderer::DrawIndexBuffer(IndexBuffer *buffer)
    {
        vkCmdDrawIndexed(GetCurrentFrame().MainCommandBuffer, buffer->GetCount(), 1, 0, 0, 0);
    }

    void VulkanRenderer::WaitForIdle()
    {
        vkDeviceWaitIdle(m_device);
    }

    std::shared_ptr<Shader> VulkanRenderer::CreateShader()
    {
        auto newShader = std::make_shared<VulkanShader>(this);
        m_shaders.push_back(newShader);
        return newShader;
    }

    std::shared_ptr<VertexBuffer> VulkanRenderer::CreateVertexBuffer()
    {
        return std::make_shared<VulkanVertexBuffer>(this);
    }

    std::shared_ptr<IndexBuffer> VulkanRenderer::CreateIndexBuffer()
    {
        return std::make_shared<VulkanIndexBuffer>(this);
    }

    std::shared_ptr<UniformBuffer> VulkanRenderer::CreateUniformBuffer()
    {
        return std::make_shared<VulkanUniformBuffer>(this);
    }

    std::shared_ptr<Texture> VulkanRenderer::CreateTexture()
    {
        return std::make_shared<VulkanTexture>(this);
    }

    void VulkanRenderer::InitCore()
    {

        std::vector<std::string> extensions{};

#if __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_MAC
        extensions.emplace_back("VK_MVK_macos_surface");
#endif
#endif
        // Initialize the instance
        vkb::InstanceBuilder builder;
        builder.set_app_name(m_rendererSettings.ApplicationName.c_str())
            .request_validation_layers(true)
            .require_api_version(1, 1, 0)
            .use_default_debug_messenger();

        for (auto &extension : extensions)
        {
            builder.enable_extension(extension.c_str());
        }

        auto builderInstance = builder.build();

        vkb::Instance vkb_inst = builderInstance.value();

        m_instance = vkb_inst.instance;
        m_debugMessenger = vkb_inst.debug_messenger;

        // request vulkan surface
        std::unordered_map<SurfaceArgs, int *> surfaceArgs{
            {SurfaceArgs::INSTANCE, (int *)m_instance},
            {SurfaceArgs::OUT_SURFACE, (int *)&m_surface}};

        ServiceLocator::GetWindow()->RequestDrawSurface(surfaceArgs);

        std::vector<std::string> deviceExtensions{};

#if __APPLE__
#if TARGET_OS_MAC
        deviceExtensions.emplace_back("VK_KHR_portability_subset");
#endif
#endif

        // Select physical device
        vkb::PhysicalDeviceSelector selector{vkb_inst};

        for (auto &extension : deviceExtensions)
        {
            selector.add_required_extension(extension.c_str());
        }

        vkb::PhysicalDevice vkbPhysicalDevice{
            selector
                .set_minimum_version(1, 1)
                .set_surface(m_surface)
                .select()
                .value()};

        // create the vulkan device
        vkb::DeviceBuilder deviceBuilder{vkbPhysicalDevice};
        vkb::Device vkbDevice{deviceBuilder.build().value()};

        m_device = vkbDevice.device;
        m_physicalDevice = vkbPhysicalDevice.physical_device;

        m_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        m_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

        VmaAllocatorCreateInfo allocatorCreateInfo{};
        allocatorCreateInfo.physicalDevice = m_physicalDevice;
        allocatorCreateInfo.device = m_device;
        allocatorCreateInfo.instance = m_instance;
        vmaCreateAllocator(&allocatorCreateInfo, &m_allocator);

        ServiceLocator::GetWindow()->RegisterWindowResizedCallback([this]()
                                                                   { m_recreateFrameBuffer = true; });
    }

    void VulkanRenderer::CleanupSwapchain()
    {
        for (auto framebuffer : m_framebuffers)
        {
            vkDestroyFramebuffer(m_device, framebuffer, nullptr);
        }

        for (auto &frame : m_frames)
        {
            vkFreeCommandBuffers(m_device, frame.CommandPool, 1, &frame.MainCommandBuffer);
        }

        vkDestroyRenderPass(m_device, m_renderPass, nullptr);

        for (auto imageView : m_swapchainImageViews)
        {
            vkDestroyImageView(m_device, imageView, nullptr);
        }

        vkDestroyImageView(m_device, m_depthImageView, nullptr);
        vmaDestroyImage(m_allocator, m_depthImage, m_depthImageAllocation);

        vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
    }

    void VulkanRenderer::RecreateSwapchain()
    {
        vkDeviceWaitIdle(m_device);
        CleanupSwapchain();

        CreateSwapchain();
        CreateCommands();
        CreateDefaultRenderPass();
        CreateFramebuffers();
        CreateDescriptorPools();

        RebuildShaders();
        // TODO: RESET SHADERS!
    }

    void VulkanRenderer::RebuildShaders()
    {
        for (const auto &shader : m_shaders)
        {
            if (auto shaderPtr = shader.lock())
            {
                reinterpret_pointer_cast<VulkanShader>(shaderPtr)->Rebuild();
            }
        }
    }

    void VulkanRenderer::BuildSwapchain()
    {

        VkSwapchainKHR oldSwapchain = m_swapchain;

        auto [width, height] = ServiceLocator::GetWindow()->GetWindowExtents();
        m_windowExtent.width = width;
        m_windowExtent.height = height;

        vkb::SwapchainBuilder swapchainBuilder{m_physicalDevice, m_device, m_surface};

        vkb::Swapchain vkbSwapchain = swapchainBuilder
                                          .use_default_format_selection()
                                          .set_desired_present_mode(VK_PRESENT_MODE_FIFO_RELAXED_KHR) // Hard VSync
                                          .set_desired_extent(width, height)
                                          .set_old_swapchain(oldSwapchain)
                                          .build()
                                          .value();

        // Store swapchain and all its related images
        m_swapchain = vkbSwapchain.swapchain;
        m_swapchainImages = vkbSwapchain.get_images().value();
        m_swapchainImageViews = vkbSwapchain.get_image_views().value();
        m_swapchainImageFormat = vkbSwapchain.image_format;

        if (oldSwapchain)
        {
            vkDestroySwapchainKHR(m_device, oldSwapchain, nullptr);
        }

        // Create depth image
        VkExtent3D depthImageExtent{
            .width = m_windowExtent.width,
            .height = m_windowExtent.height,
            .depth = 1};

        m_depthFormat = VK_FORMAT_D32_SFLOAT;

        VkImageCreateInfo depthCreateInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = m_depthFormat,
            .extent = depthImageExtent,
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT};

        VmaAllocationCreateInfo depthImageCreateInfo{
            .usage = VMA_MEMORY_USAGE_GPU_ONLY,
            .requiredFlags = VkMemoryPropertyFlags{VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT}};

        vmaCreateImage(m_allocator, &depthCreateInfo, &depthImageCreateInfo,
                       &m_depthImage, &m_depthImageAllocation, nullptr);

        VkImageViewCreateInfo depthImageViewCreateInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = m_depthImage,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = m_depthFormat,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            }};

        VK_CHECK(vkCreateImageView(m_device, &depthImageViewCreateInfo, nullptr, &m_depthImageView));
    }

    void VulkanRenderer::CreateCommands()
    {

        for (auto &frame : m_frames)
        {
            if (frame.CommandPool == VK_NULL_HANDLE)
            {
                VkCommandPoolCreateInfo commandPoolCreateInfo = VulkanInitializers::CommandPoolCreateInfo(
                    m_graphicsQueueFamily,
                    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
                VK_CHECK(vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &frame.CommandPool));
            }

            VkCommandBufferAllocateInfo commandBufferAllocateInfo = VulkanInitializers::CommandBufferAllocateInfo(
                frame.CommandPool);
            VK_CHECK(vkAllocateCommandBuffers(m_device, &commandBufferAllocateInfo, &frame.MainCommandBuffer));
        }
    }

    void VulkanRenderer::CreateDescriptorPools()
    {
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
        descriptorPoolCreateInfo.flags = 0;
        descriptorPoolCreateInfo.maxSets = 10;
        descriptorPoolCreateInfo.poolSizeCount = POOL_SIZES.size();
        descriptorPoolCreateInfo.pPoolSizes = POOL_SIZES.data();

        VK_CHECK(vkCreateDescriptorPool(m_device, &descriptorPoolCreateInfo, nullptr, &m_descriptorPool));
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
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};

        VkAttachmentReference colorAttachmentRef{
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        VkAttachmentDescription depthAttachment{
            .flags = 0,
            .format = m_depthFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

        VkAttachmentReference depthAttachmentRef{
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        VkSubpassDescription subpass{
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentRef,
            .pDepthStencilAttachment = &depthAttachmentRef};

        VkSubpassDependency depth_dependency = {};
        depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        depth_dependency.dstSubpass = 0;
        depth_dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depth_dependency.srcAccessMask = 0;
        depth_dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkSubpassDependency dependencies[]{depth_dependency};

        VkAttachmentDescription attachments[]{colorAttachment, depthAttachment};
        VkRenderPassCreateInfo renderPassCreateInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
        renderPassCreateInfo.attachmentCount = 2;
        renderPassCreateInfo.pAttachments = attachments;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = dependencies;

        VK_CHECK(vkCreateRenderPass(m_device, &renderPassCreateInfo, nullptr, &m_renderPass));
    }

    void VulkanRenderer::CreateFramebuffers()
    {
        VkFramebufferCreateInfo framebufferCreateInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        framebufferCreateInfo.renderPass = m_renderPass;
        framebufferCreateInfo.attachmentCount = 2;
        framebufferCreateInfo.width = m_windowExtent.width;
        framebufferCreateInfo.height = m_windowExtent.height;
        framebufferCreateInfo.layers = 1;

        const auto swapchainImageCount = static_cast<uint32_t>(m_swapchainImages.size());
        m_framebuffers.resize(swapchainImageCount);

        for (uint32_t i = 0; i < swapchainImageCount; i++)
        {
            VkImageView attachments[2];
            attachments[0] = m_swapchainImageViews[i];
            attachments[1] = m_depthImageView;

            framebufferCreateInfo.pAttachments = attachments;
            VK_CHECK(vkCreateFramebuffer(m_device, &framebufferCreateInfo, nullptr, &m_framebuffers[i]));
        }
    }

    void VulkanRenderer::CreateSyncStructures()
    {
        VkFenceCreateInfo fenceCreateInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkSemaphoreCreateInfo semaphoreCreateInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

        for (auto &frame : m_frames)
        {
            VK_CHECK(vkCreateFence(m_device, &fenceCreateInfo, nullptr, &frame.RenderFence));

            VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &frame.PresentSemaphore));
            VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &frame.RenderSemaphore));
        }
    }

    FrameData &VulkanRenderer::GetCurrentFrame()
    {
        return m_frames[m_frameNumber % MAX_FRAMES_IN_FLIGHT];
    }

  
}