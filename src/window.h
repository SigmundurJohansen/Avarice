#pragma once
#include <string>


namespace Avarice{
struct WindowData{
    std::string title;
    uint32_t width, height;
};
class Window{
    public:
        virtual void OpenWindow(WindowData data) = 0;
        virtual bool Update() = 0;
        virtual void RequestDrawSurface() = 0;

};
}