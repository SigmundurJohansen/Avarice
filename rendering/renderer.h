#pragma once
#include <string>
#include "shader.h"
#include <memory>

namespace Avarice
{
    struct RendererSettings
    {
        std::string ApplicationName;
    };

    class Renderer
    {
    public:
        virtual void Init(RendererSettings) = 0;
        virtual void Shutdown() = 0;
        virtual void RenderFrame() = 0;
        virtual std::shared_ptr<Shader> CreateShader() = 0;
    };
}