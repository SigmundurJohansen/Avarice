
#pragma once
#include "images.h"
#include <tuple>

namespace Avarice {
    class Texture {
    public:
        virtual ~Texture() = default;

        virtual void Bind() = 0;

        virtual void BindSamplerSettings() = 0;

        virtual void UploadData(const ImageData &data) = 0;

        [[nodiscard]] virtual std::pair<uint32_t, uint32_t> GetSize() const = 0;

        [[nodiscard]] virtual int *GetHandle() const = 0;
    };
}
