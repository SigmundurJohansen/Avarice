#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include "window.h"

#include <vulkan/vulkan.h>

namespace Avarice{
class CustomWindow : public Window
{
    public:
        CustomWindow();
        virtual void OpenWindow(WindowData _data) override;
        virtual bool Update() override;

        //virtual void RequestDrawSurface() override;
    private:
        GLFWwindow*  mWindow;
};
}