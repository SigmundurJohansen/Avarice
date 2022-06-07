#pragma once
#include <filesystem>
#include <string>

namespace Avarice
{
    class FileSystem
    {
    public:
        static inline std::filesystem::path GetRootPath()
        {
            return std::filesystem::current_path();
        }

        static inline std::filesystem::path GetAssetPath()
        {
            return GetRootPath() /= "assets";
        }

        static inline std::filesystem::path GetShaderPath()
        {
            return GetAssetPath() /= "shaders";
        }
    };
}