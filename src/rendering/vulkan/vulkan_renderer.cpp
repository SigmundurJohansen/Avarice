#include "vulkan_renderer.h"

namespace Avarice
{
    void VulkanRenderer::Init(RendererSettings _settings) 
    {
        m_RendererSettings = _settings;
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
        auto builderInstance = builder.set_app_name(m_RendererSettings.ApplicationName.c_str())
                                                    .request_validation_layers(true)
                                                    .require_api_version(1,1,0)
                                                    .use_default_debug_messenger()
                                                    .build();
        vkb::Instance vkb_inst = builderInstance.value();

        m_Instance = vkb_inst.instance;
        m_DebugMessenger = vkb_inst.debug_messenger;
        
        // request vulkan surface


    }

}