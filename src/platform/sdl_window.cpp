
#include "sdl_window.h"
#include "../core/settings_manager.h"
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <iostream>

namespace Avarice
{

    void SDLWindow::OpenWindow(WindowData data)
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            printf("Couldn't initialize SDL: %s\n", SDL_GetError());
            exit(1);
        }
        m_window = SDL_CreateWindow(data.title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, data.width, data.height, 0);
        //_window = SDL_CreateWindow(data.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, data.width, data.height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        int me = 0;
    }

    bool SDLWindow::Update()
    {
        SDL_Event event;
        SDL_PollEvent(&event);

        // Do stuff with events
        if (event.type == SDL_QUIT)
            return true;

        if (event.type == SDL_WINDOWEVENT)
        {
            switch (event.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
            {
                if (_windowResizedCallback)
                {
                    _windowResizedCallback();
                }
                break;
            }
            }
        }
        return false;
    }

    std::pair<int, int> SDLWindow::GetWindowExtents()
    {
        int width = 400;
        int height = 400;

        return {width, height};
    }

    void SDLWindow::RequestDrawSurface(std::unordered_map<SurfaceArgs, int *> args)
    {
    }
}