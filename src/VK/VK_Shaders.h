#include "Core.h"

namespace InternalVK
{
    namespace Shaders
    {
        VkShaderModule veCreateShaderModule(const std::vector<char>& code, VkDevice device) {

            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = code.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());


            VkShaderModule shaderModule;
            if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shader module!");
            }

            return shaderModule;
        }

        VkShaderModule veGetVKShader(const char* filename, VkDevice device)
        {
            std::vector<char> shaderCode;

            print((std::string)std::filesystem::current_path() + (std::string)"/src/" + filename);

            try {
                shaderCode = readFile((std::string)std::filesystem::current_path() + (std::string)"/src/" + filename);
            } catch(const std::exception& e) {
                std::cout << "Shader \"" << filename << "\" not found in ./src/\n";
                
                try {
                    shaderCode = readFile((std::string)std::filesystem::current_path() + (std::string)"/shaders/" + filename);
                } catch(const std::exception& e) {
                    std::cout << "Shader \"" << filename << "\" not found in ./shaders/\n";
                    throw std::runtime_error((std::string)"Failed to find shader \"" + filename + "\" in ./shaders nor in ./src/");
                }
            } 

            return veCreateShaderModule(shaderCode, device);
        }
    }
} 