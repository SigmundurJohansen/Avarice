#pragma once
#include <string>

namespace Avarice
{
    class Shader
    {
        public:
        virtual ~Shader() = default;
        virtual void Bind() = 0;
        virtual void Load(const std::string&& _vertexShader, const std::string&& _fragmentShader) = 0;
    };

}