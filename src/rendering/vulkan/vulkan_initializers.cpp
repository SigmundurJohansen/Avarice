#include "vulkan_initializers.h"

namespace Avarice
{
    VkCommandPoolCreateInfo VulkanInitializers::CommandPoolCreateInfo(uint32_t _queueFamilyIndex, VkCommandPoolCreateFlags _flag)
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        commandPoolCreateInfo.queueFamilyIndex = _queueFamilyIndex;
        commandPoolCreateInfo.flags = _flag;
        return commandPoolCreateInfo;
    }

    VkCommandBufferAllocateInfo VulkanInitializers::CommandBufferAllocateInfo(VkCommandPool _pool, uint32_t _count, VkCommandBufferLevel _level)
    {
        VkCommandBufferAllocateInfo commandBufferAllocateInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        commandBufferAllocateInfo.commandPool = _pool;
        commandBufferAllocateInfo.commandBufferCount = _count;
        commandBufferAllocateInfo.level = _level;
        return commandBufferAllocateInfo;
    }
}