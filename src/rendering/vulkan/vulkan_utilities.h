#pragma once
#include <iostream>
#include <vulkan/vulkan.h>

#define VK_CHECK(x)                                              \
    do                                                           \
    {                                                            \
        VkResult err = x;                                        \
        if (err)                                                 \
        {                                                        \
            std::cout << "Detected Vulkan error: " << std::endl; \
            abort();                                             \
        }                                                        \
    } while (0)

namespace Avarice
{
    class VulkanUtilities
    {
    public:
        static bool LoadShaderModule(const std::string &_filepath, VkDevice _device, VkShaderModule &_outShaderModule);
    };
    
    // from: https://stackoverflow.com/a/57595105
    template <typename T, typename... Rest>
    void hashCombine(std::size_t &seed, const T &v, const Rest &...rest)
    {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hashCombine(seed, rest), ...);
    };
}