// Standard Library
#include <stdexcept>
#include <iostream>
#include <array>
#include <chrono>

// Librarys
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "App.h"
#include "Buffer.h"
#include "Camera.h"
#include "keyboardController.h"
#include "Systems/SimpleRenderSystem.h"
#include "Systems/BillboardRenderer.h"

const glm::vec3 UP = glm::vec3(0.f, 1.f, 0.f);
float MAX_FRAME_TIME = 0.016666f; // 60 FPS

namespace FractalEngine
{

    struct GlobalUBO
    {
        alignas(16) glm::mat4 Projection{1.f};
        alignas(16) glm::mat4 View{1.f};

        alignas(16) glm::vec4 AmbientLightColor{1.f, 1.f, 1.f, 0.02f}; // The 4th Component is Intensity
        alignas(16) glm::vec4 LightPosition{-1.f};                     //  The 4th Component is Radius
        alignas(16) glm::vec4 LightColor{1.f};                         // The 4th Component is Intensity
    };

    App::App() : ViewObject{FractalGameObject::CreateGameObject()}
    {
        ViewObject.Transform.Translation = {0.f, -1.f, -2.5f};

        GlobalDescriptorPool = FractalDescriptorPool::Builder(FractalAppDevice)
                                   .setMaxSets(FractalSwapChain::MAX_FRAMES_IN_FLIGHT)
                                   .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, FractalSwapChain::MAX_FRAMES_IN_FLIGHT)
                                   .build();

        GlobalBuffers.resize(FractalSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < GlobalBuffers.size(); i++)
        {
            GlobalBuffers[i] = std::make_unique<FractalBuffer>(
                FractalAppDevice,
                sizeof(GlobalUBO),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

            GlobalBuffers[i]->map();
        }

        LoadGameObjects();
    }

    App::~App()
    {
        vkDeviceWaitIdle(FractalAppDevice.device());
    }

    void App::Run()
    {
        GlobalDescriptorSetLayout = FractalDescriptorSetLayout::Builder(FractalAppDevice)
                                        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                                        .build();

        RenderSystem = std::make_unique<FractalRenderSystem>(
            FractalAppDevice,
            FractalAppRenderer.GetSwapchainRenderPass(),
            GlobalDescriptorSetLayout->getDescriptorSetLayout());

        BillboardRenderer = std::make_unique<BillboardRenderSystem>(
            FractalAppDevice,
            FractalAppRenderer.GetSwapchainRenderPass(),
            GlobalDescriptorSetLayout->getDescriptorSetLayout());

        std::vector<VkDescriptorSet> GlobalDescriptorSets(FractalSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < GlobalDescriptorSets.size(); i++)
        {
            auto BufferInfo = GlobalBuffers[i]->descriptorInfo();
            FractalDescriptorWriter(*GlobalDescriptorSetLayout, *GlobalDescriptorPool)
                .writeBuffer(0, &BufferInfo)
                .build(GlobalDescriptorSets[i]);
        }
        std::cout << "Max push constant size = " << FractalAppDevice.properties.limits.maxPushConstantsSize << std::endl;

        auto CurrentTime = std::chrono::high_resolution_clock::now();

        while (!FractalAppWindow.ShouldClose())
        {
            glfwPollEvents();

            auto NewTime = std::chrono::high_resolution_clock::now();
            float FrameTime = std::chrono::duration<float, std::chrono::seconds::period>(NewTime - CurrentTime).count();

            CurrentTime = NewTime;
            FrameTime = std::min(FrameTime, MAX_FRAME_TIME);

            if (auto CommandBuffer = FractalAppRenderer.BeginFrame())
            {
                int FrameIndex = FractalAppRenderer.GetFrameIndex();

                Update(FrameTime, FrameIndex);

                FrameInfo frameInfo{FractalAppRenderer.GetFrameIndex(), FrameTime, CommandBuffer, Camera, GlobalDescriptorSets[FrameIndex], GameObjects};

                Render(frameInfo);

                FractalAppRenderer.EndFrame();
            }
        }
    }

    void App::LoadGameObjects()
    {
        std::shared_ptr<FractalModel> Model = FractalModel::CreateModelFromFile(FractalAppDevice, "Assets/Models/flat_vase.obj");
        auto FlatVase = FractalGameObject::CreateGameObject();
        FlatVase.Model = Model;
        FlatVase.Transform.Translation = {0.75f, 0.f, 0.f};
        FlatVase.Transform.Scale = glm::vec3(3.f, 3.f, 3.f);
        GameObjects.emplace(FlatVase.GetID(), std::move(FlatVase));

        Model = FractalModel::CreateModelFromFile(FractalAppDevice, "Assets/Models/ColoredCube.obj");
        auto ColoredCube = FractalGameObject::CreateGameObject();
        ColoredCube.Model = Model;
        ColoredCube.Transform.Translation = {0.f, -2.f, 0.f};
        ColoredCube.Transform.Scale = glm::vec3(0.3f, 0.3f, 0.3f);
        GameObjects.emplace(ColoredCube.GetID(), std::move(ColoredCube));

        Model = FractalModel::CreateModelFromFile(FractalAppDevice, "Assets/Models/smooth_vase.obj");
        auto SmoothVase = FractalGameObject::CreateGameObject();
        SmoothVase.Model = Model;
        SmoothVase.Transform.Translation = {-0.75f, 0.f, 0.};
        SmoothVase.Transform.Scale = glm::vec3(3.f, 3.f, 3.f);
        GameObjects.emplace(SmoothVase.GetID(), std::move(SmoothVase));

        Model = FractalModel::CreateModelFromFile(FractalAppDevice, "Assets/Models/Quad.obj");
        auto Floor = FractalGameObject::CreateGameObject();
        Floor.Model = Model;
        Floor.Transform.Translation = {0.f, 0.f, 0.f};
        Floor.Transform.Scale = glm::vec3(3.f, 1.f, 3.f);
        GameObjects.emplace(Floor.GetID(), std::move(Floor));

        Model = FractalModel::CreateModelFromFile(FractalAppDevice, "Assets/Models/Sphere.obj");
        auto Sphere = FractalGameObject::CreateGameObject();
        Sphere.Model = Model;
        Sphere.Transform.Translation = {0.f, -1.f, 0.f};
        Sphere.Transform.Scale = glm::vec3(3.f);
        GameObjects.emplace(Sphere.GetID(), std::move(Sphere));
    }

    void App::Update(float FrameTime, int FrameIndex)
    {
        GlobalUBO UniformBuffer{};
        UniformBuffer.Projection = Camera.GetProjection();
        UniformBuffer.View = Camera.GetView();
        GlobalBuffers[FrameIndex]->writeToBuffer(&UniformBuffer);
        GlobalBuffers[FrameIndex]->flush();

        CameraController.MoveInPlaneXZ(FractalAppWindow.GetGLFWwindow(), FrameTime, ViewObject);
        Camera.SetViewYXZ(ViewObject.Transform.Translation, ViewObject.Transform.Rotation);

        float AspectRatio = FractalAppRenderer.GetAspectRatio();
        Camera.SetPerspectiveProjection(glm::radians(60.f), AspectRatio, 0.1f, 100.f);
    }

    void App::Render(FrameInfo &FrameInfo)
    {
        FractalAppRenderer.BeginSwapChainRenderPass(FrameInfo.CommandBuffer);

        RenderSystem->RenderGameObjects(FrameInfo);
        BillboardRenderer->Render(FrameInfo);
        // PointLight->Render(FrameInfo);

        FractalAppRenderer.EndSwapChainRenderPass(FrameInfo.CommandBuffer);
    }

}