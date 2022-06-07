#pragma once
#include <../rendering/renderer.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <optional>
#include <set>
#include <SDL.h>

namespace Avarice
{
    class SDL2_Renderer : public Renderer
    {
    private:
        SDL_Renderer *m_sdl_renderer;
        SDL_Surface *m_surface = NULL;
        SDL_Window *m_window;

        static const int m_windowWidth = 1024;
        static const int m_windowHeight = 768;

    public:
        void Init(RendererSettings _settings) override;
        void Shutdown() override;
        void BeginFrame() override;
        void EndFrame() override;
        std::shared_ptr<Shader> CreateShader() override;
    };
}