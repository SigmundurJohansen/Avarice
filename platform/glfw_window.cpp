#include "glfw_window.h"

CustomWindow::CustomWindow()
{
    mWindow = nullptr;
}
void CustomWindow::OpenWindow()
{
    glfwInit();
    mWindow = glfwCreateWindow(800, 600, "Avarice Engine", nullptr, nullptr);
}
bool CustomWindow::Update()
{
    glfwPollEvents();
    return glfwWindowShouldClose(mWindow );
}