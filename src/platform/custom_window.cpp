#include <stdexcept> // for exception, runtime_error, out_of_range
#include <iostream>
#include "custom_window.h"

namespace Avarice
{

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
        //SetupImgui();
    }

    bool CustomWindow::Update()
    {
        glfwPollEvents();

        return glfwWindowShouldClose(m_Window);
    }

    std::pair<int, int> CustomWindow::GetWindowExtents()
    {
        int width, height;
        glfwGetFramebufferSize(m_Window, &width, &height);

        return {width, height};
    }

    void CustomWindow::RequestDrawSurface(std::unordered_map<SurfaceArgs, int *> args)
    {
        try
        {
            auto vkInstance = reinterpret_cast<VkInstance>(args[SurfaceArgs::INSTANCE]);
            auto *allocationCallbacks = args[SurfaceArgs::ALLOCATORS] ? reinterpret_cast<VkAllocationCallbacks *>(args[SurfaceArgs::ALLOCATORS]) : nullptr;
            auto *outSurface = reinterpret_cast<VkSurfaceKHR *>(args[SurfaceArgs::OUT_SURFACE]);

            if (vkInstance == VK_NULL_HANDLE)
            {
                throw std::runtime_error("Must provide an instance!");
            }

            if (glfwCreateWindowSurface(vkInstance, m_Window, allocationCallbacks, outSurface) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create window surface!");
            }
        }
        catch (std::bad_any_cast &e)
        {
            std::cout << "Failed to cast window surface arguments: " << e.what() << "\n";
        }
    }

}