#include "vulkan_utilities.h"
#include <fstream>
#include <vector>
#include "../platforms/filesystem.h"

namespace Avarice
{
    
    bool VulkanUtilities::LoadShaderModule(const std::string &_shaderName, VkDevice _device, VkShaderModule &_outShaderModule)
    {
        std::ifstream file(FileSystem::GetShaderPath() / _shaderName, std::ios::ate | std::ios::binary);

        if(!file.is_open())
        {
            //printf("error logging");
            return false;
        }

        size_t filesize = static_cast<size_t>(file.tellg());
        std::vector<uint32_t> buffer(filesize /sizeof(uint32_t));

        file.seekg(0);
        file.read((char*) buffer.data(), filesize);
        file.close();

        VkShaderModuleCreateInfo shaderModuleCreateInfo { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        shaderModuleCreateInfo.codeSize = buffer.size() * sizeof(uint32_t);
        shaderModuleCreateInfo.pCode = buffer.data();

        VkShaderModule shaderModule;
        if(vkCreateShaderModule(_device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            printf("cant create shader module");
            return false;
        }

        _outShaderModule = shaderModule;
        return true;

    }
}