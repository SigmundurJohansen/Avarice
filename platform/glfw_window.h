#pragma once
#include <GLFW/glfw3.h>
#include "window.h"

class CustomWindow : public Window
{
    public:
        CustomWindow();
        virtual void OpenWindow(WindowData _data) override;
        virtual bool Update() override;
    private:
        GLFWwindow*  mWindow;
};