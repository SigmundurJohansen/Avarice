#pragma once
#include <string>
#include "shader.h"
#include "buffer.h"
#include "texture.h"
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
        virtual ~Renderer() = default;
        virtual void Shutdown() = 0;
        virtual VkCommandBuffer BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void DrawIndexBuffer(IndexBuffer* buffer) = 0;
        virtual void WaitForIdle() = 0;

        virtual std::shared_ptr<Shader> CreateShader() = 0;
        virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer() = 0;
        virtual std::shared_ptr<IndexBuffer> CreateIndexBuffer() = 0;
        virtual std::shared_ptr<UniformBuffer> CreateUniformBuffer() = 0;
        virtual std::shared_ptr<Texture> CreateTexture() = 0;
    };
}