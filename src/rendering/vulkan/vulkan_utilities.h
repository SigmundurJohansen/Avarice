#pragma once
#include <iostream>

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
    static bool LoadShaderModule(const std::string &_filepath, VkDevice _device, VkShaderModule &_outShaderModule);
};

}