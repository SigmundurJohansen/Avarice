#include <stdexcept> // for exception, runtime_error, out_of_range
#include <iostream>
#include "custom_window.h"

namespace Avarice
{

    CustomWindow::CustomWindow()
    {
        m_Window = nullptr;
    }

    void CustomWindow::SetupImgui()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(m_Window, true);
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

    void CustomWindow::RequestDrawSurface(std::unordered_map<SurfaceArgs, std::any> _args)
    {
        try
        {
            auto vkInstance = std::any_cast<VkInstance>(_args[SurfaceArgs::INSTANCE]);
            //auto *allocationCallbacks = _args[SurfaceArgs::ALLOCATORS].has_value() ? std::any_cast<VkAllocationCallbacks *>(_args[SurfaceArgs::ALLOCATORS]) : nullptr;
            auto *outSurface = std::any_cast<VkSurfaceKHR *>(_args[SurfaceArgs::OUT_SURFACE]);

            if (vkInstance == VK_NULL_HANDLE)
            {
                throw std::runtime_error("Must provide both an instance!");
            }

            if (glfwCreateWindowSurface(vkInstance, m_Window, nullptr, outSurface) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create window surface!");
            }
        }
        catch (std::bad_any_cast &_error)
        {
            std::cout << "Failed to cast window surface arguments: " << _error.what() << std::endl;
        }
    }

}