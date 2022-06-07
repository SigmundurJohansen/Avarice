#include "sdl_renderer.h"
#include "service_locator.h"
#include <SDL.h>

namespace Avarice
{
    void SDL2_Renderer::Init(RendererSettings _settings)
    {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        m_sdl_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
        int oi = 0;
    }

    void SDL2_Renderer::Shutdown()
    {
        if (m_sdl_renderer)
        {
            SDL_DestroyRenderer(m_sdl_renderer);
            m_sdl_renderer = NULL;
        }

        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = NULL;
        }

        SDL_Quit();
    }

    void SDL2_Renderer::BeginFrame()
    {
        SDL_RenderClear(m_sdl_renderer);
        SDL_SetRenderDrawColor(m_sdl_renderer, 96, 128, 0, 255);

        // do something
        SDL_RenderPresent(m_sdl_renderer);
        SDL_Delay(16);
    }
    
    std::shared_ptr<Shader> SDL2_Renderer::CreateShader()
    {
        return nullptr;
    }
}