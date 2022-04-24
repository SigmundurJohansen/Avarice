#pragma once
#include <../rendering/renderer.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <optional>
#include <set>

namespace Avarice
{
    class SDL2_Renderer : public Renderer
    {
        /*
    private:
        static SDL2_Renderer m_renderer;
        SDL_Renderer* m_renderer = NULL;
		SDL_Surface* m_surface = NULL;
		SDL_Window* m_window = NULL;

		static const int m_windowWidth = 1024;
		static const int m_windowHeight = 768;
    public:
        SDL2_Renderer &GetGO() { return m_renderer; }
        void Init(RendererSettings _settings) override;
        void Shutdown() override;
        void RenderFrame() override;
        */
    };
}