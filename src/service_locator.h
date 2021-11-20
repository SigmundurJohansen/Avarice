#pragma once
#include <memory>
#include "window.h"
#include "rendering/renderer.h"

namespace Avarice{
class ServiceLocator 
{
    public:
    static inline const std::unique_ptr<Window>& GetWindow()
    {
        return m_Window;
    }
    static inline const std::unique_ptr<Renderer> &GetRenderer() {return m_Renderer;}
    static inline void Provide(Window* window)
    {
        if(m_Window != nullptr) return;
        m_Window = std::unique_ptr<Window>(window);
    }

    static inline void Provide(Renderer* _renderer, RendererSettings _rendererSettings)
    {
        if(m_Renderer != nullptr) return;

        m_Renderer = std::unique_ptr<Renderer>(_renderer);
        m_Renderer->Init(_rendererSettings);
    }

    static inline void ShutdownServices()
    {
        shutdownWindow();
    }

    private:
    static inline std::unique_ptr<Window> m_Window = nullptr;
    static inline std::unique_ptr<Renderer> m_Renderer = nullptr;

    static inline void shutdownWindow()
    {
        m_Window.reset();
        m_Window = nullptr;   
    }

};
}