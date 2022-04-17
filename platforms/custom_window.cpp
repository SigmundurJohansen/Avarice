#include "custom_window.h"
#include <stdexcept> // for exception, runtime_error, out_of_range
#include <iostream>
#include "service_locator.h"

namespace Avarice
{
    void CustomWindow::OpenWindow(WindowData _data)
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_Window = glfwCreateWindow(static_cast<int>(_data.width), static_cast<int>(_data.height), _data.title.c_str(),
                                    nullptr, nullptr);

        glfwSetWindowUserPointer(m_Window, &_input);
        for (int i = 0; i <= GLFW_JOYSTICK_LAST; i++)
        {
            if (glfwJoystickPresent(i))
            {
                std::cout << "Joystick " << i << " is present\n";
                // Register connected devices
                auto *inputManager = ServiceLocator::GetInputManager();

                if (inputManager)
                {
                    inputManager->RegisterDevice(InputDevice{
                        .Type = InputDeviceType::GAMEPAD,
                        .Index = i,
                        .StateFunc = std::bind(&CustomWindow::getGamepadState, this,
                                               std::placeholders::_1)});
                }
            }
        }

        // Register some callbacks
        glfwSetJoystickCallback([](int joystickId, int event)
                                {
                                    auto *inputManager = ServiceLocator::GetInputManager();
                                    if (inputManager)
                                    {
                                        auto *input = dynamic_cast<CustomWindow *>(ServiceLocator::GetWindow());
                                        if (input)
                                        {
                                            if (event == GLFW_CONNECTED && glfwJoystickIsGamepad(joystickId))
                                            {
                                                inputManager->RegisterDevice(InputDevice{
                                                    .Type = InputDeviceType::GAMEPAD,
                                                    .Index = joystickId,
                                                    .StateFunc = std::bind(&CustomWindow::getGamepadState, input,
                                                                           std::placeholders::_1)});
                                                std::cout << "Connected"
                                                          << "\n";
                                            }
                                            else if (event == GLFW_DISCONNECTED)
                                            {
                                                // The joystick was disconnected
                                                inputManager->RemoveDevice(InputDeviceType::GAMEPAD, joystickId);
                                                std::cout << "Disconnected"
                                                          << "\n";
                                            }
                                        }
                                    }
                                });

        glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
                           {
                               // Get the input
                               auto *input = static_cast<MultiplatformInput *>(glfwGetWindowUserPointer(window));

                               if (input)
                               {
                                   // set the new value for key
                                   float value = 0.f;

                                   switch (action)
                                   {
                                   case GLFW_PRESS:
                                   case GLFW_REPEAT:
                                       value = 1.f;
                                       break;
                                   default:
                                       value = 0.f;
                                   }

                                   input->UpdateKeyboardState(key, value);
                               }
                           });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods)
                                   {
                                       // Get the input
                                       auto *input = static_cast<MultiplatformInput *>(glfwGetWindowUserPointer(window));

                                       if (input)
                                       {
                                           input->UpdateMouseState(button, action == GLFW_PRESS ? 1.f : 0.f);
                                       }
                                   });

        // Register input devices
        auto *inputManager = ServiceLocator::GetInputManager();

        inputManager->RegisterDevice(InputDevice{
            .Type = InputDeviceType::KEYBOARD,
            .Index = 0,
            .StateFunc = std::bind(&MultiplatformInput::GetKeyboardState, &_input, std::placeholders::_1)});

        inputManager->RegisterDevice(InputDevice{
            .Type = InputDeviceType::MOUSE,
            .Index = 0,
            .StateFunc = std::bind(&MultiplatformInput::GetMouseState, &_input, std::placeholders::_1)});
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

    std::unordered_map<InputKey, InputDeviceState> CustomWindow::getGamepadState(int joystickId)
    {
        GLFWgamepadstate state;
        if (glfwGetGamepadState(joystickId, &state))
        {
            return _input.GetGamepadState(state);
        }

        return std::unordered_map<InputKey, InputDeviceState>{};
    }
}