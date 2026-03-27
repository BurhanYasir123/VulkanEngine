#include <iostream>

//#define NODEBUG
#include "VulkanEngine.h"

int main()
{
    std::cout << "\nHello, World\n";

    WindowInfo wi;
    wi.height = 600;
    wi.width = 800;
    wi.title = "Vulkan Window!!";

    auto Handle = veInit(wi);

    Log::print("UHHHHH");

    // GLM Testing

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    //std::vector<Vertex> vert = {
    //    { {-0.5f, -0.5f } , { 0.0f, 0.0f, 0.5f } },
    //    { { 0.5f ,-0.5f } , { 0.0f, 0.0f, 1.0f } },
    //    { { 0.5f , 0.5f } , { 0.0f, 0.0f, 0.5f } }
    //};
//
    //std::vector<Vertex> vert2 = {
    //    { {-0.5f, -0.5f } , { 0.0f, 0.0f, 0.5f } },
    //    { { 0.5f , 0.5f } , { 0.0f, 0.0f, 0.5f } },
    //    { {-0.5f,  0.5f } , { 1.0f, 0.0f, 1.0f } }
    //};

    //Triangle(&Handle, {200, 150}, {{1.0f, 0.0f, 0.0f},{0.0f, 1.0f, 0.0f},{0.0f, 0.0f, 1.0f}}, {100,100});
    //Triangle(&Handle, vert);
    //Triangle(&Handle, vert2);
    //Triangle(&Handle, {400, 300}, {{1.0f, 0.0f, 0.0f},{0.0f, 1.0f, 0.0f},{0.0f, 0.0f, 1.0f}}, {200,200});

    while(!glfwWindowShouldClose(Handle.Window))
    {
        vePollEvents(&Handle);
        veDrawFrame(&Handle);
        
        if(FrameCount%10 == 0) std::cout << FrameCount << '\n';
    }

    veEnd(&Handle);

    return 0;
}