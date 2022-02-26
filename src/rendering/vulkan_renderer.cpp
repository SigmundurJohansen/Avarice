#include "vulkan_renderer.h"
#include <../external/vk-bootstrap/src/VkBootstrap.h>
#include "service_locator.h"
#include "vulkan_initializers.h"
#include "vulkan_types.h"
#include "vulkan_utilities.h"
#include "vulkan_pipeline_builder.h"
#include <iostream>
#include <sys/stat.h>

namespace Avarice
{
    
    VkResult err;
static void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}


    void VulkanRenderer::Init(RendererSettings _settings)
    {
        m_rendererSettings = _settings;
        InitCore();
        CreateSwapchain();
        CreateCommands();
        CreateDefaultRenderPass();
        CreateFramebuffers();
        CreateSyncStructures();
        CreatePipelines();
        //InitImgui();
    }

    void VulkanRenderer::Shutdown()
    {
        vkDeviceWaitIdle(m_device);
        vkDestroyPipeline(m_device, m_trianglePipeline, nullptr);
        vkDestroyPipelineLayout(m_device, m_trianglePipelineLayout, nullptr);
        vkDestroyFence(m_device, m_renderFence, nullptr);
        vkDestroySemaphore(m_device, m_presentSemaphore, nullptr);
        vkDestroySemaphore(m_device, m_renderSemaphore, nullptr);

        for (auto framebuffer : m_framebuffers)
        {
            vkDestroyFramebuffer(m_device, framebuffer, nullptr);
        }
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);

        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

        for (auto imageView : m_swapchainImageViews)
        {
            vkDestroyImageView(m_device, imageView, nullptr);
        }

        vkDestroyDevice(m_device, nullptr);
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        //vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger);
        vkDestroyInstance(m_instance, nullptr);
    }

    void VulkanRenderer::RenderFrame()
    {
        VK_CHECK(vkWaitForFences(m_device, 1, &m_renderFence, true, 1000000000)); //1
        VK_CHECK(vkResetFences(m_device, 1, &m_renderFence));                     //0

        uint32_t swapchainIndex;
        VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapchain, 1000000000, m_presentSemaphore, VK_NULL_HANDLE, &swapchainIndex));
        VK_CHECK(vkResetCommandBuffer(m_mainCommandBuffer, 0));
        VkCommandBuffer cmd = m_mainCommandBuffer;
        VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

        float flashColour = abs(sin((float)m_frameNumber / 120.f));
        VkClearValue clearValue{
            .color = {0.f, 0.f, flashColour, 1.f}};

        VkRenderPassBeginInfo renderPassBeginInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
        renderPassBeginInfo.renderPass = m_renderPass;
        renderPassBeginInfo.renderArea = {
            .offset = {
                .x = 0,
                .y = 0},
            .extent = m_windowExtent};

        renderPassBeginInfo.framebuffer = m_framebuffers[swapchainIndex];
        // connect clear values
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;

        vkCmdBeginRenderPass(cmd, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // draw calls
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_trianglePipeline);
        vkCmdDraw(cmd, 3, 1, 0, 0);
        

        vkCmdEndRenderPass(cmd);           // end render pass
        VK_CHECK(vkEndCommandBuffer(cmd)); // end draw

        VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit.pWaitDstStageMask = &waitStage;

        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &m_presentSemaphore;

        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &m_renderSemaphore;

        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &m_mainCommandBuffer;

        VK_CHECK(vkQueueSubmit(m_graphicsQueue, 1, &submit, m_renderFence));

        VkPresentInfoKHR presentInfoKhr{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
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
                                   .require_api_version(1, 1, 0)
                                   .use_default_debug_messenger()
                                   .build();
        vkb::Instance vkb_inst = builderInstance.value();

        m_instance = vkb_inst.instance;
        m_debugMessenger = vkb_inst.debug_messenger;

        // request vulkan surface
        std::unordered_map<SurfaceArgs, std::any> surfaceArgs{
            {SurfaceArgs::INSTANCE, m_instance},
            {SurfaceArgs::ALLOCATORS, nullptr},
            {SurfaceArgs::OUT_SURFACE, &m_surface}};

        ServiceLocator::GetWindow()->RequestDrawSurface(surfaceArgs);

        // select physical devcice
        vkb::PhysicalDeviceSelector selector{vkb_inst};
        vkb::PhysicalDevice vkbPhysicalDevice = selector.set_minimum_version(1, 1).set_surface(m_surface).select().value();

        // create the vulkan device
        vkb::DeviceBuilder deviceBuilder{vkbPhysicalDevice};
        vkb::Device vkbDevice{deviceBuilder.build().value()};

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
        vkb::SwapchainBuilder swapchainBuilder{m_physicalDevice, m_device, m_surface};
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
        VK_CHECK(vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_commandPool));

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
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};

        VkAttachmentReference colorAttachmentRef{
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

        VkSubpassDescription subpass{
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentRef};

        VkRenderPassCreateInfo renderPassCreateInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &colorAttachment;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        VK_CHECK(vkCreateRenderPass(m_device, &renderPassCreateInfo, nullptr, &m_renderPass));
    }

    void VulkanRenderer::CreateFramebuffers()
    {
        VkFramebufferCreateInfo framebufferCreateInfo{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
        framebufferCreateInfo.renderPass = m_renderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.width = m_windowExtent.width;
        framebufferCreateInfo.height = m_windowExtent.height;
        framebufferCreateInfo.layers = 1;

        const uint32_t swapchainImageCount = m_swapchainImages.size();
        m_framebuffers.resize(swapchainImageCount);

        for (uint32_t i = 0; i < swapchainImageCount; i++)
        {
            framebufferCreateInfo.pAttachments = &m_swapchainImageViews[i];
            VK_CHECK(vkCreateFramebuffer(m_device, &framebufferCreateInfo, nullptr, &m_framebuffers[i]));
        }
    }

    void VulkanRenderer::CreateSyncStructures()
    {
        VkFenceCreateInfo fenceCreateINfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        fenceCreateINfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VK_CHECK(vkCreateFence(m_device, &fenceCreateINfo, nullptr, &m_renderFence));

        VkSemaphoreCreateInfo semaphoreCreateInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_presentSemaphore));
        VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_renderSemaphore));
    }

    void VulkanRenderer::CreatePipelines()
    {
        // debug
        /*
        struct stat info1;
        std::string path1 = "../shaders/triangle.frag.spv";
        int ret1 = stat(path1.c_str(), &info1);
        ret1 == 0 ? std::cout << "File found.\n" : std::cout << "File doesn't exist.\n";
        struct stat info2;
        std::string path2 = "../triangle.frag.spv";
        int ret2 = stat(path2.c_str(), &info2);
        ret2 == 0 ? std::cout << "File found.\n" : std::cout << "File doesn't exist.\n";
        std::ofstream outfile ("test.txt");
        outfile << "my text here!" << std::endl;
        outfile.close();
        */
        VkShaderModule triangleFragShader;
        if (!VulkanUtilities::LoadShaderModule("../external/engine/shaders/triangle.frag.spv", m_device, triangleFragShader))
        {
            std::cout << "Failed to load triangle fragment shader module\n";
        }
        else
        {
            std::cout << "Successfully loaded triangle fragment shader module\n";
        }

        VkShaderModule triangleVertShader;
        if (!VulkanUtilities::LoadShaderModule("../external/engine/shaders/triangle.vert.spv", m_device, triangleVertShader))
        {
            std::cout << "Failed to load triangle vertex shader module\n";
        }
        else
        {
            std::cout << "Successfully loaded triangle vertex shader module\n";
        }

        auto pipelineLayoutInfo = VulkanInitializers::PipelineLayoutCreateInfo();
        VK_CHECK(vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_trianglePipelineLayout));

        //TEMPORARY PIPELINE BUILDING

        VulkanPipelineBuilder pipelineBuilder;
        pipelineBuilder.m_shaderStages.push_back(
            VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, triangleVertShader));
        pipelineBuilder.m_shaderStages.push_back(
            VulkanInitializers::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));

        pipelineBuilder.m_vertexInputInfo = VulkanInitializers::PipelineVertexInputStateCreateInfo();
        pipelineBuilder.m_inputAssembly = VulkanInitializers::PipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        // build the viewport
        pipelineBuilder.m_viewport = {
            .x = 0.f,
            .y = 0.f,
            .width = static_cast<float>(m_windowExtent.width),
            .height = static_cast<float>(m_windowExtent.height),
            .minDepth = 0.f,
            .maxDepth = 1.f};

        pipelineBuilder.m_scissor = {
            .offset = {0, 0},
            .extent = m_windowExtent};

        pipelineBuilder.m_rasterizer = VulkanInitializers::PipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);
        pipelineBuilder.m_multisampling = VulkanInitializers::PipelineMultisampleStateCreateInfo();
        pipelineBuilder.m_colorBlendAttachment = VulkanInitializers::PipelineColorBlendAttachmentState();
        pipelineBuilder.m_pipelineLayout = m_trianglePipelineLayout;

        m_trianglePipeline = pipelineBuilder.BuildPipeline(m_device, m_renderPass);

        vkDestroyShaderModule(m_device, triangleFragShader, nullptr);
        vkDestroyShaderModule(m_device, triangleVertShader, nullptr);
    }
/*
    void VulkanRenderer::InitImgui()
    {
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        
        err = vkCreateDescriptorPool(m_device, &pool_info, VK_NULL_HANDLE, &m_descriptorPool);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = m_instance;
        init_info.PhysicalDevice = m_physicalDevice;
        init_info.Device = m_device;
        init_info.QueueFamily = m_graphicsQueueFamily;
        init_info.Queue = m_graphicsQueue;
        //init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = m_descriptorPool;
        init_info.Subpass = 0;
        init_info.MinImageCount = 2;
        init_info.ImageCount = 2;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = VK_NULL_HANDLE;
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info, m_renderPass);
        

        err = vkResetCommandPool(m_device, m_commandPool, 0);
        check_vk_result(err);
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(m_mainCommandBuffer, &begin_info);
        check_vk_result(err);

        ImGui_ImplVulkan_CreateFontsTexture(m_mainCommandBuffer);

        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &m_mainCommandBuffer;
        err = vkEndCommandBuffer(m_mainCommandBuffer);
        check_vk_result(err);
        err = vkQueueSubmit(m_graphicsQueue, 1, &end_info, VK_NULL_HANDLE);
        check_vk_result(err);

        err = vkDeviceWaitIdle(m_device);
        check_vk_result(err);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
*/

}