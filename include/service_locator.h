#pragma once
#include <memory>
#include "../platform/window.h"
#include "../rendering/renderer.h"
#include "../inputs/input_manager.h"

namespace Avarice
{
    class ServiceLocator
    {
    public:
        static inline Window *GetWindow() { return m_Window.get(); }
        static inline Renderer *GetRenderer() { return m_Renderer.get(); }
        static inline InputManager *GetInputManager() { return m_InputManager.get(); }

        static inline void Provide(Window *window)
        {
            if (m_Window != nullptr)
                return;
            m_Window = std::unique_ptr<Window>(window);
            int oi = 0;
        }

        static inline void Provide(Renderer *_renderer, RendererSettings _rendererSettings)
        {
            if (m_Renderer != nullptr)
                return;

            m_Renderer = std::unique_ptr<Renderer>(_renderer);
            m_Renderer->Init(_rendererSettings);
        }

        static inline void Provide(InputManager *_inputManager)
        {
            if (m_InputManager != nullptr)
                return;
            m_InputManager = std::unique_ptr<InputManager>(_inputManager);
        }

        static inline void ShutdownServices()
        {
            ShutdownInputManager();
            ShutdownRenderer();
            ShutdownWindow();
        }

    private:
        static inline std::unique_ptr<Window> m_Window = nullptr;
        static inline std::unique_ptr<Renderer> m_Renderer = nullptr;
        static inline std::unique_ptr<InputManager> m_InputManager = nullptr;

        static inline void ShutdownWindow()
        {
            m_Window.reset();
            m_Window = nullptr;
        }

        static inline void ShutdownRenderer()
        {
            if (!m_Renderer)
                return;
            m_Renderer->Shutdown();
            m_Renderer = nullptr;
        }

        static inline void ShutdownInputManager()
        {
            if (!m_InputManager)
                return;

            m_InputManager.reset();
        }
    };
}