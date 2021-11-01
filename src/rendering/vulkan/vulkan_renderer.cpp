#include "vulkan_renderer.h"
#include <vulkan/vulkan.h>

namespace Avarice
{
    void VulkanRenderer::Init(RendererSettings _settings) 
    {
        mRendererSettings = _settings;
        InitCore();
    }
    void VulkanRenderer::Shutdown()
    {

    }
    void VulkanRenderer::RenderFrame()
    {

    }
    void VulkanRenderer::InitCore()
    {
        vkb::InstanceBuilder builder;
        /*
        auto builderInstance = builder.set_app_name(mRendererSettings.ApplicationName.c_str())
                                                    .request_validation_layers(true)
                                                    .require_api_version(1,1,0)
                                                    .use_default_debug_messenger()
                                                    .build();
        vkb::Instance vkb_inst = builderInstance.value();

        mInstance = vkb_inst.instance;
        mDebugMessenger = vkb_inst.debug_messenger;
        */
        // request vulkan surface


    }

}