#include "glfw_window.h"


namespace Avarice{
CustomWindow::CustomWindow()
{
    mWindow = nullptr;
}
void CustomWindow::OpenWindow(WindowData _data)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    mWindow = glfwCreateWindow(_data.width, _data.height, _data.title.c_str(), nullptr, nullptr);
}
bool CustomWindow::Update()
{
    glfwPollEvents();
    return glfwWindowShouldClose(mWindow );
}
}