#pragma once

#include <filesystem>

namespace Avarice
{
    class FileSystem
    {
        public:
        static inline std::filesystem::path GetRootPath()
        {
            return std::filesystem::current_path();

        }

        static inline std::filesystem::path GetShaderPath()
        {
            return GetRootPath();
        } 
    };
}