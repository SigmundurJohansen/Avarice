#pragma once
#include <memory>
#include "window.h"

class ServiceLocator 
{
    public:
    static inline const std::unique_ptr<Window>& GetWindow()
    {
        return mWindow;
    }

    static inline void Provide(Window* window)
    {
        if(mWindow != nullptr) return;
        mWindow = std::unique_ptr<Window>(window);
    }

    static inline void ShutdownServices()
    {
        shutdownWindow();
    }

    private:
    static inline std::unique_ptr<Window> mWindow = nullptr;

    static inline void shutdownWindow()
    {
        mWindow.reset();
        mWindow = nullptr;   
    }

};