#include "glfw_window.h"

CustomWindow::CustomWindow()
{
    mWindow = nullptr;
}
void CustomWindow::OpenWindow(WindowData _data)
{
    glfwInit();
    mWindow = glfwCreateWindow(_data.width, _data.height, _data.title.c_str(), nullptr, nullptr);
}
bool CustomWindow::Update()
{
    glfwPollEvents();
    return glfwWindowShouldClose(mWindow );
}