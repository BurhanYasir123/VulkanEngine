#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/vec4.hpp>
//#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <map>
#include <optional>
#include <thread>
#include <chrono>
#include <set>
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
#include <fstream>
#include <filesystem>
#include <array>

#define VK_VULKAN_ENGINE_VERSION VK_MAKE_VERSION(1, 0, 0)

#ifndef APP_VERSION_MAJOR
#define APP_VERSION_MAJOR 1
#endif

#ifndef APP_VERSION_MINOR
#define APP_VERSION_MINOR 0
#endif

#ifndef APP_VERSION_REVISION
#define APP_VERSION_REVISION 0
#endif

#define APP_VERSION VK_MAKE_VERSION(APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_REVISION)

#define VE_SUCCESS 0
#define VE_FAILURE -1

#include "Log.h"
using namespace Log;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NODEBUG
    const bool enableValidationLayers = false;
    const bool DEBUG = false;
#else
    const bool enableValidationLayers = true;
    //const bool enableValidationLayers = false;
    const bool DEBUG = true;
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "[VALIDATION_LAYER]: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}


struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct WindowInfo{
    WindowInfo() {}
    int width;
    int height;
    const char* title;
};

struct QueueFamilyindices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete()
    {
        //if(DEBUG)
        //{
        //    std::cout << "GraphicsFamily: " << (graphicsFamily.has_value() ? "Yes" : "No") << "\n";
        //    std::cout << "PresentFamily: " << (presentFamily.has_value() ? "Yes" : "No") << "\n";
        //}

        //return graphicsFamily.has_value() || presentFamily.has_value();
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct VulkanRegistry{
    VulkanRegistry() {}
    GLFWwindow* Window;
    WindowInfo WinInfo;
    VkInstance Instance = VK_NULL_HANDLE;
    VkSurfaceKHR Surface = VK_NULL_HANDLE;
    std::vector<VkExtensionProperties> SupportedInstanceEXTs;
    std::vector<VkExtensionProperties> SupportedDeviceEXTs;
    std::vector<VkLayerProperties> SupportedLayers;
    std::vector<VkQueueFamilyProperties> QueueFamilies;
    QueueFamilyindices Queueindices;
    VkDebugUtilsMessengerEXT DebugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice LogicalDevice;
    VkQueue DeviceQueue;
    VkQueue PresentQueue;
    SwapChainSupportDetails SwapChainDetails;
    VkBool32 presentSupport = false;
    VkSwapchainKHR SwapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer; 
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

};


struct GLFWextensionInfo
{
    GLFWextensionInfo() {}
    uint32_t glfwextensionCount;
    const char** glfwextensions;
};


struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);


        return attributeDescriptions;
    }
};

struct VeBuffer
{
    VeBuffer(VkBuffer Buffer, VkDeviceMemory BufferMemory, uint32_t vertexCount) : m_Buffer(Buffer), m_BufferMemory(BufferMemory), m_vertexCount(vertexCount) {ID=-1;}
    VeBuffer() {ID=-1;}
    VkBuffer m_Buffer;
    VkDeviceMemory m_BufferMemory;
    uint32_t m_vertexCount;
    int ID;
    void DeleteItself(VkDevice LogicalDevice)
    {
        vkDestroyBuffer(LogicalDevice, m_Buffer, nullptr);
        vkFreeMemory(LogicalDevice, m_BufferMemory, nullptr);
    }
};

std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error(((std::string)"failed to open file!  " + filename).c_str());
    }

    size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties)
          == properties) {
            return i;
        }
    }


    throw std::runtime_error("failed to find suitable memory type!");
}

std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};


std::vector<Vertex> vertices2 = {
    { {-0.5f,  -0.83f } , { 1.0f, 0.0f, 0.0f } },
    { {-0.25f, -0.83f } , { 0.0f, 1.0f, 0.5f } },
    { {-0.75f, -0.83f } , { 0.0f, 0.0f, 1.0f } }
};

std::vector<VeBuffer> VBuffers = {

};
std::vector<VeBuffer> IBuffers = {

};


int FrameCount;
