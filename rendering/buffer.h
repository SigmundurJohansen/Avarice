#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <cassert>
#include "types.h"

namespace Avarice {

    class VertexBuffer {
    public:
        virtual ~VertexBuffer() = default;
        virtual void Bind() = 0;
        virtual void UploadData(const std::vector<Vertex>&) = 0;

        virtual uint64_t GetCount() = 0;
    };

    class IndexBuffer {
    public:
        virtual ~IndexBuffer() = default;
        virtual void Bind() = 0;
        virtual void UploadData(const std::vector<uint32_t>&) = 0;
        virtual uint64_t GetCount() = 0;
    };

    class UniformBuffer {
    public:
        virtual ~UniformBuffer() = default;
        virtual void Bind() = 0;
        virtual void UploadData(const UniformBufferObject&) = 0;
    };
}