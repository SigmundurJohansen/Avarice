
#pragma once
#include <unordered_map>
#include "../inputs/input_key.h"
#include "../inputs/input_devices.h"
#include <GLFW/glfw3.h>

namespace Avarice {
    class MultiplatformInput {
    public:

        std::unordered_map<InputKey, InputDeviceState> GetKeyboardState(int index) { return _keyboardState; }
        std::unordered_map<InputKey, InputDeviceState> GetMouseState(int index) { return _mouseState; }
        std::unordered_map<InputKey, InputDeviceState> GetGamepadState(const GLFWgamepadstate& state);

        void UpdateKeyboardState(int key, float value);
        void UpdateMouseState(int button, float value);

    private:
        static InputKey multiplatformKeyToInputKey(int key);
        static InputKey multiplatformMouseButtonToInputKey(int button);
    private:
        std::unordered_map<InputKey, InputDeviceState> _keyboardState {};
        std::unordered_map<InputKey, InputDeviceState> _mouseState {};

    };
}
