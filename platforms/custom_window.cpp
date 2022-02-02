#include <stdexcept>   // for exception, runtime_error, out_of_range
#include <iostream>
#include "custom_window.h"

namespace Avarice{

CustomWindow::CustomWindow()
{
    m_Window = nullptr;
}

void CustomWindow::OpenWindow(WindowData _data)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_Window = glfwCreateWindow(_data.width, _data.height, _data.title.c_str(), nullptr, nullptr);
}

bool CustomWindow::Update()
{
    glfwPollEvents();
    return glfwWindowShouldClose(m_Window );
}

std::pair<int,int> CustomWindow::GetWindowExtents()
{
    int width, height;
    glfwGetFramebufferSize(m_Window, &width,&height);

    return {width,height};
}

void CustomWindow::RequestDrawSurface(std::unordered_map<SurfaceArgs, std::any> _args)
{
    try{
        auto vkInstance = std::any_cast<VkInstance>(_args[SurfaceArgs::INSTANCE]);
        //auto *allocationCallbacks = _args[SurfaceArgs::ALLOCATORS].has_value() ? std::any_cast<VkAllocationCallbacks *>(_args[SurfaceArgs::ALLOCATORS]) : nullptr;
        auto *outSurface = std::any_cast<VkSurfaceKHR*>(_args[SurfaceArgs::OUT_SURFACE]);

        if(vkInstance == VK_NULL_HANDLE)
        {
            throw std::runtime_error("Must provide both an instance!");
        }

        if(glfwCreateWindowSurface(vkInstance, m_Window, nullptr, outSurface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface!");
        }
    } catch(std::bad_any_cast& _error)
    {
        std::cout << "Failed to cast window surface arguments: " << _error.what() << std::endl;
    }
}

}