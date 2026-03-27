#include "VK_Shaders.h"

namespace InternalVK
{
    uint32_t veGetSupportedInstanceExtensionsCount()
    {
        uint32_t EXTcount;
        vkEnumerateInstanceExtensionProperties(nullptr, &EXTcount, nullptr);
        return EXTcount;
    }
    
    uint32_t veGetSupportedInstanceLayersCount()
    {
        uint32_t LayersCount;
        vkEnumerateInstanceLayerProperties(&LayersCount, nullptr);  
        return LayersCount;
    }
    
    std::vector<VkExtensionProperties> veGetSupportedInstanceExtensions()
    {
        uint32_t EXTcount = veGetSupportedInstanceExtensionsCount(); 
        std::vector<VkExtensionProperties> EXT(EXTcount);
        vkEnumerateInstanceExtensionProperties(nullptr, &EXTcount, EXT.data());
        return EXT;
    }
    
    std::vector<VkExtensionProperties> veGetSupportedDeviceExtensions(VulkanRegistry* Handle)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(Handle->physicalDevice, nullptr, &extensionCount, nullptr);
    
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(Handle->physicalDevice, nullptr, &extensionCount, availableExtensions.data());
    
        return availableExtensions;
    }
    
    std::vector<VkLayerProperties> veGetSupportedInstanceLayers()
    {
        uint32_t LayersCount = veGetSupportedInstanceLayersCount();
        std::vector<VkLayerProperties> Layers(LayersCount);
        vkEnumerateInstanceLayerProperties(&LayersCount, Layers.data());
        return Layers;
    }
    
    GLFWextensionInfo veGetGLFWRequiredExtensions()
    {
        GLFWextensionInfo EXTInfo;
        EXTInfo.glfwextensions = glfwGetRequiredInstanceExtensions(&EXTInfo.glfwextensionCount);
        return EXTInfo;
    }
    
    std::vector<VkQueueFamilyProperties> veGetQueueFamiliesFromPhysicalDevice(VkPhysicalDevice physicalDevice)
    {
        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
    
        return queueFamilies;
    }
    
    //QueueFamilyindices vePickOutGraphicsCompatibleFamilyQueues(std::vector<VkQueueFamilyProperties> QueueFamilies)
    QueueFamilyindices vePickOutGraphicsCompatibleFamilyQueues(VulkanRegistry* Handle)
    {
        QueueFamilyindices indices;
    
        int i = 0;
        
        for (const auto& queueFamily : Handle->QueueFamilies)
        {
    
            if(DEBUG) print(STR + "QueueFamily[" + std::to_string(i) + "]: "
              + "Graphics=" + ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) ? "Yes" : "No"));
    
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(Handle->physicalDevice, i, Handle->Surface, &presentSupport);
            
            if (presentSupport) {
                indices.presentFamily = i;
            }
    
            Handle->presentSupport = presentSupport;
    
            print(STR + "PRESENT_SUPPORT=" + (presentSupport ? "True" : "False"));
    
            if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }
    
            i++;
        }
    
        return indices;
    }
    
    VkDevice veCreateLogicalDevice(VulkanRegistry* Handle)
    {
        VkDevice device;
        float queuePriority = 1.0f;
    
        QueueFamilyindices indices = Handle->Queueindices;
    
    
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
        
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
    
        
        VkPhysicalDeviceFeatures deviceFeatures{};
        VkDeviceCreateInfo createInfo{};
    
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    
    
    
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
    
        VkResult result = vkCreateDevice(Handle->physicalDevice, &createInfo, nullptr, &device);
        if(result != VK_SUCCESS)
        {
            throw std::runtime_error("[VULKAN ERROR] Failed to create Logical Device"); 
        } else if(result == VK_SUCCESS)
        {
            print("Logical Device Created!!");
        }
    
        return device;
    }
    
    std::vector<const char*> veGetRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    
        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
    
        return extensions;
    }
    
    VkQueue veGetGraphicsDeviceQueue(VulkanRegistry* Handle)
    {
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        QueueFamilyindices indices = Handle->Queueindices;
    
        vkGetDeviceQueue(Handle->LogicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
    
        print("Got Graphics Device Queue From Device(duh)");
    
        return graphicsQueue;
    }
    
    VkQueue veGetPresentDeviceQueue(VulkanRegistry* Handle)
    {
        VkQueue presentQueue;
        QueueFamilyindices indices = Handle->Queueindices;
    
        vkGetDeviceQueue(Handle->LogicalDevice, indices.presentFamily.value(), 0, &presentQueue);
    
        print("Got Presentation Device Queue From Device(again... duh)");
    
        return presentQueue;
    }
    
    VkSurfaceKHR veCreateKHRSurface(VulkanRegistry* Handle)
    {
        try {
            VkSurfaceKHR surface = VK_NULL_HANDLE;
    
            VkResult result = glfwCreateWindowSurface(Handle->Instance, Handle->Window, nullptr, &surface);
    
            if (result != VK_SUCCESS) throw std::runtime_error("Could not create KHR_Surface");
            if (surface == VK_NULL_HANDLE) throw std::runtime_error("KHR_Surface is null after creation");
    
            print("KHR_Surface created!");
            return surface;
        } catch (const std::exception& e) {
            std::cerr << "[EXCEPTION] " << e.what() << "\n";
            exit(1);
        }
    }
    
    bool veCheckDeviceExtensionSupport(VulkanRegistry* Handle)
    {
        std::set<std::string> requiredextensions(deviceExtensions.begin(), deviceExtensions.end());
    
        for(const auto& extension : Handle->SupportedDeviceEXTs)
        {
            requiredextensions.erase(extension.extensionName);
        }
    
        return requiredextensions.empty();
    }
    
    
    SwapChainSupportDetails veQuerySwapChainSupport(VulkanRegistry* Handle) {
        SwapChainSupportDetails details;
    
        // Get Surface Capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Handle->physicalDevice, Handle->Surface, &details.capabilities);
    
    
        uint32_t formatCount;
    
        // Get DeviceSurfaceFormats Count, i think it means the pixel formats 
        vkGetPhysicalDeviceSurfaceFormatsKHR(Handle->physicalDevice, Handle->Surface, &formatCount, nullptr);
        
        if (formatCount != 0) {
            details.formats.resize(formatCount);
    
            // Get DeviceSurfaceFormats Actually this time
            vkGetPhysicalDeviceSurfaceFormatsKHR(Handle->physicalDevice, Handle->Surface, &formatCount, details.formats.data());
        }
    
        uint32_t presentModeCount;
    
        // Now in the exact same way, query the PresentModes Count
        vkGetPhysicalDeviceSurfacePresentModesKHR(Handle->physicalDevice, Handle->Surface, &presentModeCount, nullptr);
        
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
    
            // and now query the actual PresentModes
            vkGetPhysicalDeviceSurfacePresentModesKHR(Handle->physicalDevice, Handle->Surface, &presentModeCount, details.presentModes.data());
        }
    
        return details;
    }
    
    VkSurfaceFormatKHR veChooseSwapSurfaceFormat(SwapChainSupportDetails SwapChainDetails) {
        for (const auto& availableFormat : SwapChainDetails.formats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return SwapChainDetails.formats[0];
    }
    
    // Probably the simplest function in this entire program, because i am to lazy for implementing detection,
    // Nevermind 
    // TODO: Implement Detection
    VkPresentModeKHR veChooseSwapPresentMode(SwapChainSupportDetails SwapChainDetails) {
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    
    VkExtent2D veChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
    
            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };
    
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    
            return actualExtent;
        }
    }
    
    void veCreateSwapChain(VulkanRegistry* Handle)
    {
    
        if(Handle->Surface == VK_NULL_HANDLE) throw std::runtime_error("veCreateSwapChain(): Surface is Null");
        if(!Handle->Window) throw std::runtime_error("veCreateSwapChain(): Window is null");
        if(!Handle->Queueindices.IsComplete()) throw std::runtime_error("veCreateSwapChain(): Queueindices.IsComplete() did not return true");
        if(Handle->LogicalDevice == VK_NULL_HANDLE) throw std::runtime_error("veCreateSwapChain(): LogicalDevice is Null");
    
        print("Createing SwapChain...:");
    
        VkSwapchainKHR swapChain;
    
        VkSurfaceFormatKHR surfaceFormat = veChooseSwapSurfaceFormat(Handle->SwapChainDetails);
        VkPresentModeKHR presentMode = veChooseSwapPresentMode(Handle->SwapChainDetails);
        VkExtent2D extent = veChooseSwapExtent(Handle->SwapChainDetails.capabilities, Handle->Window);
    
        uint32_t imageCount = Handle->SwapChainDetails.capabilities.minImageCount + 1;
    
        if (Handle->SwapChainDetails.capabilities.maxImageCount > 0 &&
             imageCount > Handle->SwapChainDetails.capabilities.maxImageCount) 
        {
            imageCount = Handle->SwapChainDetails.capabilities.maxImageCount;
        }
    
        // oh boy
    
        uint32_t queueFamilyIndices[] = {Handle->Queueindices.graphicsFamily.value(), Handle->Queueindices.presentFamily.value()};
    
        VkSwapchainCreateInfoKHR createInfo{};
        
        print("\tFilling the CreateInfo...");
    
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = Handle->Surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (Handle->Queueindices.graphicsFamily != Handle->Queueindices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }
    
        createInfo.preTransform = Handle->SwapChainDetails.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;
    
        print("\tRunning vkCreateSwapchainKHR()...");
        if (vkCreateSwapchainKHR(Handle->LogicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }
    
        print("\tGetting SwapChainImages...");
        
        vkGetSwapchainImagesKHR(Handle->LogicalDevice, swapChain, &imageCount, nullptr);
        Handle->swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(Handle->LogicalDevice, swapChain, &imageCount, Handle->swapChainImages.data());
    
        Handle->swapChainImageFormat = surfaceFormat.format;
        Handle->swapChainExtent = extent;
    
        print("\t----[ Success ]----\n");
    
        Handle->SwapChain = swapChain;
    }
    
    void veCreateSwapChainImageViews(VulkanRegistry* Handle)
    {
        Handle->swapChainImageViews.resize(Handle->swapChainImages.size());
    
        for (size_t i = 0; i < Handle->swapChainImages.size(); i++) {
    
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = Handle->swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = Handle->swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
    
    
            if (vkCreateImageView(Handle->LogicalDevice, &createInfo, nullptr, &(Handle->swapChainImageViews[i])) != VK_SUCCESS) {
               throw std::runtime_error("failed to create image views!");
            }
        }
    }

    bool isDeviceSuitable(VulkanRegistry* Handle) {
        QueueFamilyindices indices = vePickOutGraphicsCompatibleFamilyQueues(Handle);

        bool extensionsSupported = veCheckDeviceExtensionSupport(Handle);
        //bool extensionsSupported = true;

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = Handle->SwapChainDetails;
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        print(STR +  
            "isDeviceSuitable():\n" + 
            "\tGraphics and Present Families are " + (indices.IsComplete() ? "Supported\n" : "Not Supported\n") +
            "\tRequired Device Extentions are " + (extensionsSupported ? "Present\n" : "Not Present\n") +
            "\tThe SwapChain is " + (swapChainAdequate ? "Adequate\n" : "Not Adequate\n"));



        return indices.IsComplete() && extensionsSupported && swapChainAdequate;
    }
    
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    void destroyDebugMessennger(VulkanRegistry* Handle)
    {
        if (enableValidationLayers) {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
                vkGetInstanceProcAddr(Handle->Instance, "vkDestroyDebugUtilsMessengerEXT");
    
            if (func != nullptr) {
                func(Handle->Instance, Handle->DebugMessenger, nullptr);
            }
        }
    }

    VkResult createDebugUtilsMessengerEXT(VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pMessenger) {

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }


    bool checkValidationLayerSupport()
    {
        auto availableLayers = veGetSupportedInstanceLayers();
        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
        
            if (!layerFound) {
                return false;
            }
        }

        return true;
    }
    VkInstance createInstance(VulkanRegistry* Handle)
    {   
        if (enableValidationLayers && !InternalVK::checkValidationLayerSupport()) {
            throw std::runtime_error("[VULKAN ERROR] validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = Handle->WinInfo.title;
        appInfo.applicationVersion = APP_VERSION;
        appInfo.pEngineName        = "VulkanEngine";
        appInfo.engineVersion      = VK_VULKAN_ENGINE_VERSION;
        appInfo.apiVersion         = VK_API_VERSION_1_0;

        std::vector<const char*> extensions = veGetRequiredExtensions();

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        VkInstance instance;
        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("[VULKAN ERROR] Error Creating Instance");
        } else if(result == VK_SUCCESS)
        {
            std::cout << "Instance Created!!\n";
        }

        if(instance == VK_NULL_HANDLE)
        {
            std::cout << "INSTANCE NULL IN createInstance()\n";
        }

        if (enableValidationLayers) {
            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
            populateDebugMessengerCreateInfo(debugCreateInfo);

            if (createDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &Handle->DebugMessenger) != VK_SUCCESS) {
                throw std::runtime_error("[VULKAN ERROR] Failed to set up debug messenger!");
            }
        }

        return instance;
    }

    VkPhysicalDevice pickPhysicalDevice(VulkanRegistry* Handle)
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(Handle->Instance, &deviceCount, nullptr);

        std::cout << "Amount of Devices Found: " << deviceCount << '\n';

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        if (Handle->Instance == VK_NULL_HANDLE)
            std::cerr << "Instance is null before vkEnumeratePhysicalDevices!\n";


        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(Handle->Instance, &deviceCount, devices.data());

        bool deviceFound = false;

        int i = 0;
        for (const auto& device : devices) {
            std::cout << i << 'n';
            Handle->physicalDevice = device;

            if (Handle->physicalDevice == VK_NULL_HANDLE) {
                std::cerr << "[DEBUG] physicalDevice is NULL before queue scan\n";
            }
            if (Handle->Surface == VK_NULL_HANDLE) {
                std::cerr << "[DEBUG] Surface is NULL before queue scan\n";
            }

            if(device != VK_NULL_HANDLE)
            {
                auto physicalDevice = device; // temp assign
                auto QueueFamilies = veGetQueueFamiliesFromPhysicalDevice(physicalDevice);
                Handle->physicalDevice = device;
                auto DeviceEXTs = veGetSupportedDeviceExtensions(Handle);
                Handle->SupportedDeviceEXTs = DeviceEXTs;
                Handle->QueueFamilies = QueueFamilies;
                auto SwapChainDetails = veQuerySwapChainSupport(Handle);
                Handle->SwapChainDetails = SwapChainDetails;
                if(isDeviceSuitable(Handle)) {
                    std::cout << "Found a Compatible physical Device!\n";
                    deviceFound = true;
                    return device;
                    break;
                }
            }
            i++;
        }

        if(!deviceFound){
            throw std::runtime_error("failed to find a suitable GPU!");
            return 0;
        }
    }

    void veCreateFramebuffers(VulkanRegistry* Handle)
    {
        Handle->swapChainFramebuffers.resize(Handle->swapChainImageViews.size());

        for (size_t i = 0; i < Handle->swapChainImageViews.size(); i++) {
            VkImageView attachments[] = {
                Handle->swapChainImageViews[i]
            };
        
            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = Handle->renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = Handle->swapChainExtent.width;
            framebufferInfo.height = Handle->swapChainExtent.height;
            framebufferInfo.layers = 1;
        
            if (vkCreateFramebuffer(Handle->LogicalDevice, &framebufferInfo, nullptr, &Handle->swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void veCreateCommandPool(VulkanRegistry* Handle)
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = Handle->Queueindices.graphicsFamily.value();

        if (vkCreateCommandPool(Handle->LogicalDevice, &poolInfo, nullptr, &Handle->commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void veCreateCommandBuffer(VulkanRegistry* Handle)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = Handle->commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;
            
        if (vkAllocateCommandBuffers(Handle->LogicalDevice, &allocInfo, &Handle->commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void veRecordCommandBuffer(VulkanRegistry* Handle, uint32_t imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional
            
        if (vkBeginCommandBuffer(Handle->commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = Handle->renderPass;
        renderPassInfo.framebuffer = Handle->swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = Handle->swapChainExtent;
        
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;


        vkCmdBeginRenderPass(Handle->commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(Handle->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Handle->graphicsPipeline);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(Handle->swapChainExtent.width);
            viewport.height = static_cast<float>(Handle->swapChainExtent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(Handle->commandBuffer, 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = Handle->swapChainExtent;
            vkCmdSetScissor(Handle->commandBuffer, 0, 1, &scissor);


            VkDeviceSize offsets[] = {0};
            for(auto buffer : VBuffers)
            {
                VkBuffer vb[] = { buffer.m_Buffer };
                auto IB = IBuffers[buffer.ID];
                if((FrameCount%10) == 0) std::cout << "Vertex Buffer Registered\n";
                vkCmdBindVertexBuffers(Handle->commandBuffer, 0, 1, vb, offsets);
                vkCmdBindIndexBuffer(Handle->commandBuffer, IB.m_Buffer, 0, VK_INDEX_TYPE_UINT16);
                vkCmdDrawIndexed(Handle->commandBuffer, static_cast<uint32_t>(IB.m_vertexCount), 1, 0, 0, 0);
            }
            if((FrameCount%10) == 0)std::cout << "\n";


        vkCmdEndRenderPass(Handle->commandBuffer);


        if (vkEndCommandBuffer(Handle->commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void veCreateSyncObjects(VulkanRegistry* Handle)
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(Handle->LogicalDevice, &semaphoreInfo, nullptr, &Handle->imageAvailableSemaphore) !=
            VK_SUCCESS ||
            vkCreateSemaphore(Handle->LogicalDevice, &semaphoreInfo, nullptr, &Handle->renderFinishedSemaphore) !=
                VK_SUCCESS ||
            vkCreateFence(Handle->LogicalDevice, &fenceInfo, nullptr, &Handle->inFlightFence) != VK_SUCCESS) {
            throw std::runtime_error("failed to create semaphores!");
        }
    }

    void veRegisterVeVBuffer(VeBuffer buffer)
    {
        VBuffers.push_back(buffer);
    }

    void veRegisterVeIBuffer(VeBuffer buffer)
    {
        IBuffers.push_back(buffer);
    }

    template <typename T>
    void veCopyDataToBuffer(VkDevice LogicalDevice, VkDeviceMemory vertexBufferMemory, VkDeviceSize BufferSize, std::vector<T> dataArray)
    {
        void* data;
        vkMapMemory(LogicalDevice, vertexBufferMemory, 0, BufferSize, 0, &data);
            memcpy(data, dataArray.data(), (size_t) BufferSize);
        vkUnmapMemory(LogicalDevice, vertexBufferMemory);
    }

    void veCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VulkanRegistry* Handle) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = Handle->commandPool;
        allocInfo.commandBufferCount = 1;
    
        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(Handle->LogicalDevice, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        
        vkQueueSubmit(Handle->DeviceQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(Handle->DeviceQueue);

        vkFreeCommandBuffers(Handle->LogicalDevice, Handle->commandPool, 1, &commandBuffer);
    }


    void veCreateVkBuffer(VkBuffer* VB, VkDeviceMemory* vertexBufferMemory, VkDevice LogicalDevice,
        VkPhysicalDevice physicalDevice, VkDeviceSize BufferSize,  VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = BufferSize;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(LogicalDevice, &bufferInfo, nullptr, VB) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(LogicalDevice, *VB, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);
            
            
        if (vkAllocateMemory(LogicalDevice, &allocInfo, nullptr, vertexBufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }
    }

    template <typename T>
    VeBuffer veCreateVertexBufferFromArray(VulkanRegistry* Handle, std::vector<T> dataArray)
    {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        VkBuffer VB;
        VkDeviceMemory vertexBufferMemory;

        VkDeviceSize BufferSize = sizeof(dataArray[0]) * dataArray.size();

        veCreateVkBuffer(&stagingBuffer, &stagingBufferMemory, Handle->LogicalDevice, Handle->physicalDevice, BufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        vkBindBufferMemory(Handle->LogicalDevice, stagingBuffer, stagingBufferMemory, 0);
        
        veCopyDataToBuffer(Handle->LogicalDevice, stagingBufferMemory, BufferSize, dataArray);

        veCreateVkBuffer(&VB, &vertexBufferMemory, Handle->LogicalDevice, Handle->physicalDevice, BufferSize, 
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vkBindBufferMemory(Handle->LogicalDevice, VB, vertexBufferMemory, 0);

        veCopyBuffer(stagingBuffer, VB, BufferSize, Handle);

        vkDestroyBuffer(Handle->LogicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(Handle->LogicalDevice, stagingBufferMemory, nullptr);

        return VeBuffer(VB, vertexBufferMemory, dataArray.size());
    }

    VeBuffer veCreateIndexBufferFromArray(VulkanRegistry* Handle, std::vector<uint16_t> data)
    {
        VkDeviceSize bufferSize = sizeof(data[0]) * data.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        

        veCreateVkBuffer(&stagingBuffer, &stagingBufferMemory, Handle->LogicalDevice, Handle->physicalDevice, bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
        vkBindBufferMemory(Handle->LogicalDevice, stagingBuffer, stagingBufferMemory, 0);

        void* data_;
        vkMapMemory(Handle->LogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data_);
        memcpy(data_, indices.data(), (size_t) bufferSize);
        vkUnmapMemory(Handle->LogicalDevice, stagingBufferMemory);
    
        veCreateVkBuffer(&indexBuffer, &indexBufferMemory, Handle->LogicalDevice, Handle->physicalDevice, bufferSize, 
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    
        vkBindBufferMemory(Handle->LogicalDevice, indexBuffer, indexBufferMemory, 0);

        veCopyBuffer(stagingBuffer, indexBuffer, bufferSize, Handle);
    
        vkDestroyBuffer(Handle->LogicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(Handle->LogicalDevice, stagingBufferMemory, nullptr);

        return VeBuffer(indexBuffer, indexBufferMemory, data.size());
    }

    namespace Shaders
    {

        void veCreateRenderPass(VulkanRegistry* Handle)
        {
            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = Handle->swapChainImageFormat;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colorAttachmentRef{};
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;

            VkSubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = 1;
            renderPassInfo.pAttachments = &colorAttachment;
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;
            renderPassInfo.dependencyCount = 1;
            renderPassInfo.pDependencies = &dependency;

            if (vkCreateRenderPass(Handle->LogicalDevice, &renderPassInfo, nullptr, &Handle->renderPass) != VK_SUCCESS) {
                throw std::runtime_error("failed to create render pass!");
            }
        }

        void veCreateGraphicsPipeline(VulkanRegistry* Handle)
        {
            VkShaderModule vertShaderModule = veGetVKShader("vert.spv", Handle->LogicalDevice);
            VkShaderModule fragShaderModule = veGetVKShader("frag.spv", Handle->LogicalDevice);
            Handle->vertShaderModule = vertShaderModule;
            Handle->fragShaderModule = fragShaderModule;

            VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertShaderStageInfo.module = vertShaderModule;
            vertShaderStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
            fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragShaderStageInfo.module = fragShaderModule;
            fragShaderStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

            
            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            
            auto bindingDescription = Vertex::getBindingDescription();
            auto attributeDescriptions = Vertex::getAttributeDescriptions();
            
            std::cout << "Binding=" << bindingDescription.binding
                      << " stride=" << bindingDescription.stride << "\n";
            for (auto& attr : attributeDescriptions) {
                std::cout << "Attr loc=" << attr.location
                          << " binding=" << attr.binding
                          << " offset=" << attr.offset << "\n";
            }

            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.vertexAttributeDescriptionCount =
                static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
            vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();


            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float) Handle->swapChainExtent.width;
            viewport.height = (float) Handle->swapChainExtent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            
            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = Handle->swapChainExtent;

            std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };
            
            VkPipelineDynamicStateCreateInfo dynamicState{};
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
            dynamicState.pDynamicStates = dynamicStates.data();

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.scissorCount = 1;

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f;
            rasterizer.depthBiasClamp = 0.0f;
            rasterizer.depthBiasSlopeFactor = 0.0f;

            VkPipelineMultisampleStateCreateInfo multisampling{};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisampling.minSampleShading = 1.0f; // Optional
            multisampling.pSampleMask = nullptr; // Optional
            multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
            multisampling.alphaToOneEnable = VK_FALSE; // Optional

            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

            VkPipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional

            // -------------TODO------------- SEPERATE TO ANOTHER FUNCTION

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 0; // Optional
            pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
            pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
            pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

            if (vkCreatePipelineLayout(Handle->LogicalDevice, &pipelineLayoutInfo, nullptr, &Handle->pipelineLayout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout!");
            }
            // -----------------------------------------------------------------

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = nullptr; // Optional
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = &dynamicState;
            pipelineInfo.layout = Handle->pipelineLayout;
            pipelineInfo.renderPass = Handle->renderPass;
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
            pipelineInfo.basePipelineIndex = -1; // Optional

            if (vkCreateGraphicsPipelines(Handle->LogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &Handle->graphicsPipeline) != VK_SUCCESS) {
                throw std::runtime_error("failed to create graphics pipeline!");
            }

        }
    }

}