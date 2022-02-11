#pragma once
#include <thread>
#include <vulkan/vulkan.h>

namespace Avarice
{
    class CommandPool
    {
    public:
        explicit CommandPool(const std::thread::id &threadId = std::this_thread::get_id());

        ~CommandPool();

        operator const VkCommandPool &() const { return commandPool; }

        const VkCommandPool &GetCommandPool() const { return commandPool; }
        const std::thread::id &GetThreadId() const { return threadId; }

    private:
        VkCommandPool commandPool = VK_NULL_HANDLE;
        std::thread::id threadId;
    };
}
