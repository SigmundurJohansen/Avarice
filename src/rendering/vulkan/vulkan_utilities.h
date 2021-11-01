#pragma once
#include <iostream>

#define VK_CHECK(x)                                                 \
    do{                                                             \
        VkResult err = x;                                           \
        if(err)                                                     \
        {                                                           \
            std::cout << "Detected VUlkan error: " < << std::endl;  \
            abort();                                                \
        }                                                           \
    }while(0)                                                       \

    class VulkanUtilities
    {

    };