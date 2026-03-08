#pragma once

#define GLFW_INCLUDE_VULKAN

// Standard Headers
#include <string>

// Vulkan Headers
#include <GLFW/glfw3.h>

namespace FractalEngine
{

    class FractalWindow
    {

    public:
        FractalWindow(int W, int H, std::string Name);
        ~FractalWindow();

        FractalWindow(const FractalWindow &) = delete;
        FractalWindow &operator=(const FractalWindow &) = delete;

        bool ShouldClose() { return glfwWindowShouldClose(Window); }
        VkExtent2D GetExtent() { return {static_cast<uint32_t>(Width), static_cast<uint32_t>(Height)}; }

        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    private:
        void InitWindow();

        const int Width;
        const int Height;

        std::string WindowName;

        GLFWwindow *Window;
    };
}