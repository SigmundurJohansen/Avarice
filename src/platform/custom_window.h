#pragma once
#define GLFW_INCLUDE_VULKAN

#include "../platform/window.h"
#include <GLFW/glfw3.h>

namespace Avarice
{
    class CustomWindow : public Window
    {
    public:
        CustomWindow();
        void OpenWindow(WindowData _data) override;
        bool Update() override;

        std::pair<int, int> GetWindowExtents() override;
        void RequestDrawSurface(std::unordered_map<SurfaceArgs, int*> _args) override;
        void RegisterWindowResizedCallback(std::function<void()> callback) override { _resizeCallback = callback; }

    private:
        GLFWwindow *m_Window;

        
        std::function<void()> _resizeCallback{};
    };
}