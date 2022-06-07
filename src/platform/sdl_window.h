
#pragma once
#include "../platform/window.h"
#include <SDL.h>

namespace Avarice
{
    class SDLWindow : public Window
    {
    public:
        SDLWindow()= default;
        void OpenWindow(WindowData data) override;
        bool Update() override;

        std::pair<int, int> GetWindowExtents() override;
        void RequestDrawSurface(std::unordered_map<SurfaceArgs, int *> args) override;
        void RegisterWindowResizedCallback(std::function<void()> function) override
        {
            _windowResizedCallback = function;
        }

    private:
        SDL_Window *m_window = nullptr;
        std::function<void()> _windowResizedCallback;
    };
}
