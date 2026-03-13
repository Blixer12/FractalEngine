// Standard Library
#include <stdexcept>
#include <iostream>
#include <array>

// Librarys
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "App.h"
#include "SimpleRenderSystem.h"

namespace FractalEngine
{
    App::App()
    {
        LoadGameObjects();
    }

    App::~App()
    {
        vkDeviceWaitIdle(FractalAppDevice.device());
    }

    void App::Run()
    {
        std::cout << "Max push constant size = " << FractalAppDevice.properties.limits.maxPushConstantsSize << std::endl;

        while (!FractalAppWindow.ShouldClose())
        {
            glfwPollEvents();
            Render();
        }

        vkDeviceWaitIdle(FractalAppDevice.device());
    }

    void App::LoadGameObjects()
    {
        std::vector<FractalModel::Vertex> Vertices{
            {{0, -0.5f}, {1, 0, 0, 1}},
            {{0.5f, 0.5f}, {0, 1, 0, 1}},
            {{-0.5f, 0.5f}, {0, 0, 1, 1}}};

        auto FractalAppModel = std::make_shared<FractalModel>(FractalAppDevice, Vertices);

        auto Triangle = FractalGameObject::CreateGameObject();
        Triangle.Model = FractalAppModel;
        Triangle.Color = {.1f, .8f, .1f, 1};
        Triangle.Transform2D.Translation.x = .2f;
        Triangle.Transform2D.Scale = {2, 0.5f};
        Triangle.Transform2D.Rotation = 0.25f * glm::two_pi<float>();

        GameObjects.push_back(std::move(Triangle));
    }

    void App::Render()
    {
        FractalRenderSystem RenderSystem{FractalAppDevice, FractalAppRenderer.GetSwapchainRenderPass()};

        if (auto CommandBuffer = FractalAppRenderer.BeginFrame())
        {
            FractalAppRenderer.BeginSwapChainRenderPass(CommandBuffer);

            RenderSystem.RenderGameObjects(CommandBuffer, GameObjects);

            FractalAppRenderer.EndSwapChainRenderPass(CommandBuffer);

            FractalAppRenderer.EndFrame();
        }
    }

}