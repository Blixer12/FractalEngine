// Standard Library
#include <stdexcept>

#include "Window.h"

namespace FractalEngine
{

    FractalWindow::FractalWindow(int W, int H, std::string Name) : Width{W}, Height{H}, WindowName{Name}
    {
        InitWindow();
    }

    FractalWindow::~FractalWindow()
    {
        glfwDestroyWindow(Window);
        glfwTerminate();
    }

    void FractalWindow::InitWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        Window = glfwCreateWindow(Width, Height, WindowName.c_str(), nullptr, nullptr);
    }

    void FractalWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, Window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed To Create Window Surface");
        }
    }

}
