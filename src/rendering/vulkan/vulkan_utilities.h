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

class VulkanUtilities
{

};