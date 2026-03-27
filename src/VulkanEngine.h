#pragma once

#include "Core.h"
#include "VK.h"

// class Triangle
// {
// public:
//     std::vector<Vertex> ColorPosSizeToVertices(glm::vec3 color, glm::vec2 pos, glm::vec2 size)
//     {
//         // std::cout << "WinInfo: width=" << m_GlobalReg->WinInfo.width
//         //   << " height=" << m_GlobalReg->WinInfo.height << '\n';
//         // std::cout << "pos=(" << pos.x << "," << pos.y << ") size=(" << size.x << "," << size.y << ")\n";


//         // std::cout << "Triangle positions:\n"
//         //   << "P1: (" << (((pos.x)/m_GlobalReg->WinInfo.width)*2 - 1) << ", "
//         //              << (((pos.y - size.y)/m_GlobalReg->WinInfo.height)*2 - 1) << ")\n"
//         //   << "P2: (" << (((pos.x + size.x)/m_GlobalReg->WinInfo.width)*2 - 1) << ", "
//         //              << (((pos.y + size.y)/m_GlobalReg->WinInfo.height)*2 - 1) << ")\n"
//         //   << "P3: (" << (((pos.x - size.x)/m_GlobalReg->WinInfo.width)*2 - 1) << ", "
//         //              << (((pos.y + size.y)/m_GlobalReg->WinInfo.height)*2 - 1) << ")\n";


//         if(MultipleColors)
//         {
//             return {
//                 { {(((pos.x)/m_GlobalReg->WinInfo.width)*2)-1, (((pos.y-size.y)/m_GlobalReg->WinInfo.height)*2)-1}, colors[0] },
//                 { {(((pos.x+size.x)/m_GlobalReg->WinInfo.width)*2)-1, (((pos.y+size.y)/m_GlobalReg->WinInfo.height)*2)-1}, colors[1] },
//                 { {(((pos.x-size.x)/m_GlobalReg->WinInfo.width)*2)-1, (((pos.y+size.y)/m_GlobalReg->WinInfo.height)*2)-1}, colors[2] }
//             };
//         }
//         else {
//             return {
//                 { {(((pos.x)/m_GlobalReg->WinInfo.width)*2)-1, (((pos.y-size.y)/m_GlobalReg->WinInfo.height)*2)-1}, color },
//                 { {(((pos.x+size.x)/m_GlobalReg->WinInfo.width)*2)-1, (((pos.y+size.y)/m_GlobalReg->WinInfo.height)*2)-1}, color },
//                 { {(((pos.x-size.x)/m_GlobalReg->WinInfo.width)*2)-1, (((pos.y+size.y)/m_GlobalReg->WinInfo.height)*2)-1}, color }
//             };
//         }
//     }

//     Triangle(VulkanRegistry* Handle, glm::vec2 pos, glm::vec3 color, glm::vec2 size) {
//         MultipleColors = false;
//         m_pos = pos;
//         m_color = color;
//         m_size = size;
//         m_GlobalReg = Handle;
//         vertices = ColorPosSizeToVertices(color, pos, size);

//         VertexBuffer = InternalVK::veCreateVertexBufferFromArray(Handle->LogicalDevice, Handle->physicalDevice, vertices);
//         InternalVK::veRegisterVeBuffer(VertexBuffer);
//         ID=-1;
//     }

//     Triangle(VulkanRegistry* Handle, glm::vec2 pos, std::vector<glm::vec3> _colors, glm::vec2 size) {
//         MultipleColors = true;
//         m_pos = pos;
//         colors = _colors;
//         m_size = size;
//         m_GlobalReg = Handle;
//         vertices = ColorPosSizeToVertices(m_color, pos, size);

//         VertexBuffer = InternalVK::veCreateVertexBufferFromArray(Handle->LogicalDevice, Handle->physicalDevice, vertices);
//         InternalVK::veRegisterVeBuffer(VertexBuffer);
//         ID=-1;
//     }

//     Triangle(VulkanRegistry* Handle, std::vector<Vertex> _vertices)
//     {
//         MultipleColors = false;
//         vertices = _vertices;

//         // std::cout << "Triangle created with " << vertices.size() << " vertices\n";
//         // for (auto& v : vertices) {
//         //     std::cout << "  (" << v.pos.x << "," << v.pos.y << ") color("
//         //               << v.color.r << "," << v.color.g << "," << v.color.b << ")\n";
//         // }


//         VertexBuffer = InternalVK::veCreateVertexBufferFromArray(Handle->LogicalDevice, Handle->physicalDevice, vertices);
//         InternalVK::veRegisterVeBuffer(VertexBuffer);
//         ID=-1;
//     }
    
//     VulkanRegistry* m_GlobalReg;
//     std::vector<Vertex> vertices;
//     glm::vec2 m_pos;
//     glm::vec3 m_color;
//     bool MultipleColors;
//     std::vector<glm::vec3> colors;
//     glm::vec2 m_size;
//     VeBuffer VertexBuffer;
//     int ID;

// };


// std::vector<Vertex> vertices = {
//     { {-0.5f, -0.5f } , { 1.0f, 0.0f, 0.0f } },
//     { { 0.0f , 0.5f } , { 0.0f, 1.0f, 0.0f } },
//     { { 0.5f , 0.5f } , { 0.0f, 0.0f, 1.0f } }
// };

void vePollEvents(VulkanRegistry* Handle)
{
    glfwPollEvents();
}

VulkanRegistry veInit(WindowInfo winInfo)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(winInfo.width, winInfo.height, winInfo.title, nullptr, nullptr);
    if(!window) throw std::runtime_error("Window could not be created");
    glfwPollEvents(); // Mabye help to do something???? 

    print(std::to_string(InternalVK::veGetSupportedInstanceExtensionsCount()) + " instance extensions supported!");

    FrameCount = 0;

    // Populate VulkanRegistry

    VulkanRegistry Reg;
    Reg.Window                     = window;
    Reg.WinInfo                    = winInfo;
    Reg.Instance                   = InternalVK::createInstance(&Reg);
    Reg.Surface                    = InternalVK::veCreateKHRSurface(&Reg);
    Reg.SupportedInstanceEXTs      = InternalVK::veGetSupportedInstanceExtensions();
    Reg.SupportedLayers            = InternalVK::veGetSupportedInstanceLayers();
    Reg.physicalDevice             = InternalVK::pickPhysicalDevice(&Reg);
    Reg.SupportedDeviceEXTs        = InternalVK::veGetSupportedDeviceExtensions(&Reg);
    Reg.QueueFamilies              = InternalVK::veGetQueueFamiliesFromPhysicalDevice(Reg.physicalDevice);
    Reg.Queueindices               = InternalVK::vePickOutGraphicsCompatibleFamilyQueues(&Reg);
    Reg.Queueindices.presentFamily = Reg.Queueindices.graphicsFamily;
    Reg.SwapChainDetails           = InternalVK::veQuerySwapChainSupport(&Reg);
    Reg.LogicalDevice              = InternalVK::veCreateLogicalDevice(&Reg);
    Reg.DeviceQueue                = InternalVK::veGetGraphicsDeviceQueue(&Reg);
    Reg.PresentQueue               = InternalVK::veGetPresentDeviceQueue(&Reg);
    InternalVK::veCreateSwapChain(&Reg);
    InternalVK::veCreateSwapChainImageViews(&Reg);
    InternalVK::Shaders::veCreateRenderPass(&Reg);
    InternalVK::Shaders::veCreateGraphicsPipeline(&Reg);
    InternalVK::veCreateFramebuffers(&Reg);
    InternalVK::veCreateCommandPool(&Reg);
    IBuffers.push_back(InternalVK::veCreateIndexBufferFromArray(&Reg, indices));
    auto vb = InternalVK::veCreateVertexBufferFromArray(&Reg, vertices);
    vb.ID = IBuffers.size() - 1;
    VBuffers.push_back(vb);
    //Buffers.push_back(InternalVK::veCreateVertexBufferFromArray(Reg.LogicalDevice, Reg.physicalDevice, vertices2));
    //Triangle tri = Triangle(&Reg, vertices);
    InternalVK::veCreateCommandBuffer(&Reg);
    InternalVK::veCreateSyncObjects(&Reg);

    // Debug Messages

#ifndef NODEBUG

    print("Running Checks...:\n");
    print(STR + "\tWindow is"          + ( (Reg.Window)                              ? " Okay" : " Not Okay"));
    print(STR + "\tInstance is"        + ( (Reg.Instance != VK_NULL_HANDLE)          ? " Okay" : " Not Okay"));
    print(STR + "\tSurface is"         + ( (Reg.Surface != VK_NULL_HANDLE)           ? " Okay" : " Not Okay"));
    print(STR + "\tPhysical Device is" + ( (Reg.physicalDevice != VK_NULL_HANDLE)    ? " Okay" : " Not Okay"));
    print(STR + "\tQueueIndicies is"   + ( (Reg.Queueindices.IsComplete())           ? " Okay" : " Not Okay"));
    print(STR + "\tLogical Device is"  + ( (Reg.LogicalDevice != VK_NULL_HANDLE)     ? " Okay" : " Not Okay"));
    print(STR + "\tDevice Queue is"    + ( (Reg.DeviceQueue != VK_NULL_HANDLE)       ? " Okay" : " Not Okay"));
    print(STR + "\tPresent Queue is"   + ( (Reg.PresentQueue != VK_NULL_HANDLE)      ? " Okay" : " Not Okay"));
    print(STR + "\tSwapChain is"       + ( (Reg.SwapChain != VK_NULL_HANDLE)         ? " Okay" : " Not Okay"));
    print(STR + "\tPipeline is"        + ( (Reg.graphicsPipeline != VK_NULL_HANDLE)  ? " Okay" : " Not Okay"));
    print("\t-----[ CHECKS DONE ]-----\n");

    if(Reg.Queueindices.presentFamily.value() == Reg.Queueindices.graphicsFamily.value())
    {
        print("PresentFamily == GraphicsFamily");
    } else {
        print("PresentFamily != GraphicsFamily");
    }

    // std::cout << "Available Instance Extensions:\n";
    // for (const auto& extension : Reg.SupportedInstanceEXTs) {
    //     std::cout << '\t' << extension.extensionName << '\n';
    // }
    // std::cout << '\n'; 

    // std::cout << "Available Device Extensions:\n";
    // for (const auto& extension : Reg.SupportedDeviceEXTs) {
    //     std::cout << '\t' << extension.extensionName << '\n';
    // }
    // std::cout << '\n';

    // std::cout << "Available Layers Extensions:\n";
    // for (const auto& layer : Reg.SupportedLayers) {
    //     std::cout << '\t' << layer.layerName << '\n';
    // }
    // std::cout << '\n';

    // dont really need the if statement cuz itll crash if it isnt supported but oh well
    if(InternalVK::veCheckDeviceExtensionSupport(&Reg))
    {
        print("RequiredDeviceExtentions are supported, so that means:");
        for (const auto& extension : deviceExtensions)
        {
            print(STR + "\t" + extension + " is Supported");
        }
        print("","");
    }

    print(STR + "Validation layers" + (InternalVK::checkValidationLayerSupport() ? " are" : " are NOT") + " supported");

    print("Avalible Queue Families:");
    for(const auto& queueFamily : Reg.QueueFamilies)
    {
        print(STR + "\tGraphicsCompatible( " 
            + (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT ? "True" : "False") 
            + ")  FamilyQueueCount: " + std::to_string(queueFamily.queueCount));
    }
    print("", "");

#endif
    return Reg;
}

void veDrawFrame(VulkanRegistry* Handle)
{
    vkWaitForFences(Handle->LogicalDevice, 1, &Handle->inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(Handle->LogicalDevice, 1, &Handle->inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(Handle->LogicalDevice, Handle->SwapChain, UINT64_MAX, Handle->imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(Handle->commandBuffer, 0);

    InternalVK::veRecordCommandBuffer(Handle, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {Handle->imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &Handle->commandBuffer;
    
    VkSemaphore signalSemaphores[] = {Handle->renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(Handle->DeviceQueue, 1, &submitInfo, Handle->inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {Handle->SwapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(Handle->PresentQueue, &presentInfo);

    FrameCount += 1;
}

void veEnd(VulkanRegistry* Handle){

    vkDeviceWaitIdle(Handle->LogicalDevice);

    
    InternalVK::destroyDebugMessennger(Handle);

    for (auto framebuffer : Handle->swapChainFramebuffers) {
        vkDestroyFramebuffer(Handle->LogicalDevice, framebuffer, nullptr);
    }
    for (auto imageView : Handle->swapChainImageViews) {
        vkDestroyImageView(Handle->LogicalDevice, imageView, nullptr);
    }
    for (auto Buffer : VBuffers) {
        // vkDestroyBuffer(Handle->LogicalDevice, Buffer.m_Buffer, nullptr);
        // vkFreeMemory(Handle->LogicalDevice, Buffer.m_BufferMemory, nullptr);
        Buffer.DeleteItself(Handle->LogicalDevice);
    }
    for (auto Buffer : IBuffers) {
        // vkDestroyBuffer(Handle->LogicalDevice, Buffer.m_Buffer, nullptr);
        // vkFreeMemory(Handle->LogicalDevice, Buffer.m_BufferMemory, nullptr);
        Buffer.DeleteItself(Handle->LogicalDevice);
    }


    vkDestroySemaphore(Handle->LogicalDevice, Handle->imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(Handle->LogicalDevice, Handle->renderFinishedSemaphore, nullptr);
    vkDestroyFence(Handle->LogicalDevice, Handle->inFlightFence, nullptr);
    vkDestroyCommandPool(Handle->LogicalDevice, Handle->commandPool, nullptr);
    vkDestroyPipeline(Handle->LogicalDevice, Handle->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(Handle->LogicalDevice, Handle->pipelineLayout, nullptr);
    vkDestroyRenderPass(Handle->LogicalDevice, Handle->renderPass, nullptr);
    vkDestroyShaderModule(Handle->LogicalDevice, Handle->fragShaderModule, nullptr);
    vkDestroyShaderModule(Handle->LogicalDevice, Handle->vertShaderModule, nullptr);
    vkDestroySwapchainKHR(Handle->LogicalDevice, Handle->SwapChain, nullptr);
    vkDestroySurfaceKHR(Handle->Instance, Handle->Surface, nullptr);
    vkDestroyDevice(Handle->LogicalDevice, nullptr);
    vkDestroyInstance(Handle->Instance, nullptr);
    
    glfwDestroyWindow(Handle->Window);
    glfwTerminate();
}