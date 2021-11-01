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
        return mWindow;
    }
    static inline const std::unique_ptr<Renderer> &GetRenderer() {return mRenderer;}
    static inline void Provide(Window* window)
    {
        if(mWindow != nullptr) return;
        mWindow = std::unique_ptr<Window>(window);
    }

    static inline void Provide(Renderer* _renderer, RendererSettings _rendererSettings)
    {
        if(mRenderer != nullptr) return;

        mRenderer = std::unique_ptr<Renderer>(_renderer);
        mRenderer->Init(_rendererSettings);
    }

    static inline void ShutdownServices()
    {
        shutdownWindow();
    }

    private:
    static inline std::unique_ptr<Window> mWindow = nullptr;
    static inline std::unique_ptr<Renderer> mRenderer = nullptr;

    static inline void shutdownWindow()
    {
        mWindow.reset();
        mWindow = nullptr;   
    }

};
}