#pragma once
#include <memory>

namespace Avarice
{
    class SettingsManager
    {
    public:
        bool GetIsVulkanEnabled() { return isVulkanEnabled; }
        void SetIsVulkanEnabled(bool _usingVulkan) { isVulkanEnabled = _usingVulkan; }

        SettingsManager(SettingsManager const &) = delete;
        SettingsManager &operator=(SettingsManager const &) = delete;
        static SettingsManager &GetInstance()
        {
            static SettingsManager m_instance;
            return m_instance;
        }

    private:
        SettingsManager();
        bool isVulkanEnabled = true;
    };

}
