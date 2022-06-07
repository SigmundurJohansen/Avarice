#pragma once
#include <string>
#include <memory>
#include "buffer.h"
#include "texture.h"

namespace Avarice
{
    class Shader
    {
        public:
        virtual ~Shader() = default;
        virtual void Bind() = 0;
        virtual void Load(const std::string&& _vertexShader, const std::string&& _fragmentShader) = 0;
        virtual void AddUniformBuffer(std::shared_ptr<UniformBuffer> buffer) = 0;
        virtual void AddTexture(std::shared_ptr<Texture> texture) = 0;
    };
}