#pragma once
#include <iostream>
#include <vulkan/vulkan.h>
#include "vulkan_includes.h"


#define VK_CHECK(x)                                                 \
    do{                                                             \
        VkResult err = x;                                           \
        if(err)                                                     \
        {                                                           \
            std::cout << "Detected Vulkan error: " << std::endl;    \
            abort();                                                \
        }                                                           \
    }while(0)                                                       \
    
namespace Avarice
{    
class VulkanUtilities
{
    public:
    static bool LoadShaderModule(const std::string& _filepath, VkDevice _device, VkShaderModule &_outShaderModule);
};
}