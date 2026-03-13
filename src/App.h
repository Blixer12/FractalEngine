#pragma once

// Standard Library
#include <memory>
#include <vector>

#include "Device.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Window.h"

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
        void Render();

        FractalWindow FractalAppWindow{Width, Height, "Fractal Engine"};
        FractalDevice FractalAppDevice{FractalAppWindow};
        FractalRenderer FractalAppRenderer{FractalAppWindow, FractalAppDevice};

        std::vector<FractalGameObject> GameObjects;
    };
}