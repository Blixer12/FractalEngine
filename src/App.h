#pragma once

// Standard Library
#include <memory>
#include <vector>

#include "Descriptors.h"
#include "Device.h"
#include "GameObject.h"
#include "KeyboardController.h"
#include "Renderer.h"
#include "Window.h"
#include "SimpleRenderSystem.h"

namespace FractalEngine
{
    class App
    {

    public:
        static constexpr int Width = 1280;
        static constexpr int Height = 720;

        App();
        ~App();

        App(const App &) = delete;
        App &operator=(const App &) = delete;

        void Run();

    private:
        void LoadGameObjects();
        void Render(FrameInfo &FrameInfo);
        void Update(float FrameTime, int FrameIndex);

        FractalWindow FractalAppWindow{Width, Height, "Fractal Engine"};
        FractalDevice FractalAppDevice{FractalAppWindow};
        FractalRenderer FractalAppRenderer{FractalAppWindow, FractalAppDevice};

        std::unique_ptr<FractalRenderSystem> RenderSystem{};
        std::unique_ptr<BillboardRenderSystem> BillboardRenderer{};

        FractalCamera Camera{};
        std::vector<std::unique_ptr<FractalBuffer>> GlobalBuffers;

        KeyboardController CameraController{};
        FractalGameObject ViewObject;

        // Note: Order of declaration matters, as GlobalDescriptorPool must be destrtoyed before Device
        std::unique_ptr<FractalDescriptorPool> GlobalDescriptorPool;
        std::unique_ptr<FractalDescriptorSetLayout> GlobalDescriptorSetLayout;

        FractalGameObject::Map GameObjects;
    };
}