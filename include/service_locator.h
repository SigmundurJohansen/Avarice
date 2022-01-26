#pragma once
#include <memory>
#include "window.h"
#include "../rendering/renderer.h"
#include "../input/input_manager.h"

namespace Avarice{
class ServiceLocator 
{
    public:
    static inline const std::unique_ptr<Window> &GetWindow() { return m_Window; }
    static inline const std::unique_ptr<Renderer> &GetRenderer() {return m_Renderer;}
    static inline InputManager* GetInputManager() { return m_inputManager.get(); }

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

    static inline void Provide(InputManager* _inputManager) {
        if (m_inputManager != nullptr) return;
        m_inputManager = std::unique_ptr<InputManager>(_inputManager);
    }

    static inline void ShutdownServices()
    {
        shutdownInputManager();
        ShutdownRenderer();
        ShutdownWindow();
    }

    private:
    static inline std::unique_ptr<Window> m_Window = nullptr;
    static inline std::unique_ptr<Renderer> m_Renderer = nullptr;
    static inline std::unique_ptr<InputManager> m_inputManager = nullptr;

    static inline void ShutdownWindow()
    {
        m_Window.reset();
        m_Window = nullptr;   
    }

    static inline void ShutdownRenderer()
    {
        if(!m_Renderer) 
            return;
        m_Renderer->Shutdown();   
        m_Renderer = nullptr;
    }

    static inline void shutdownInputManager() {
        if (!m_inputManager) return;

        m_inputManager.reset();
    }
};
}