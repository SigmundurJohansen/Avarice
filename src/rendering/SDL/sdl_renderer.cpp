#include "sdl_renderer.h"
#include "service_locator.h"
#include <SDL.h>

namespace Avarice
{
    void SDL2_Renderer::Init(RendererSettings _settings)
    {
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
            printf("Unable to Init hinting: %s", SDL_GetError());

        if ((m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED)) == NULL)
            printf("Unable to create renderer");

        SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0xFF);
    }

    void SDL2_Renderer::Shutdown()
    {
        if (m_renderer)
        {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = NULL;
        }

        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = NULL;
        }

        SDL_Quit();
    }

    void SDL2_Renderer::RenderFrame()
    {
        SDL_RenderClear(m_renderer);
        // do something
        SDL_RenderPresent(m_renderer);
    }
}