#pragma once
#define GLFW_INCLUDE_VULKAN

#include "window.h"
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
        void RequestDrawSurface(std::unordered_map<SurfaceArgs, std::any> _args) override;
        void SetupImgui();

    private:
        GLFWwindow *m_Window;
    };
}