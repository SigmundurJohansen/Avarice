#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include "window.h"

namespace Avarice{
class CustomWindow : public Window
{
    public:
        CustomWindow();
        virtual void OpenWindow(WindowData _data) override;
        virtual bool Update() override;

        virtual void RequestDrawSurface(std::unordered_map<SurfaceArgs, std::any> _args) override;
        std::pair<int,int> GetWindowExtents() override;
    private:
        GLFWwindow*  m_Window;
};
}