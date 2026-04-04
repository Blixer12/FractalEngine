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
        bool WasWindowResized() { return FrameBufferResized; }
        void ResetWindowResizedFlag() { FrameBufferResized = false; }
        GLFWwindow *GetGLFWwindow() const { return Window; }

        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    private:
        static void FrameBufferResizeCallback(GLFWwindow *Window, int Width, int Height);
        void InitWindow();

        int Width;
        int Height;
        bool FrameBufferResized = false;

        std::string WindowName;

        GLFWwindow *Window;
    };
}