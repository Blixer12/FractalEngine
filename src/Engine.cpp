// Standard Library Headers
#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include "App.h"
#include "Window.h"

int main()
{
    FractalEngine::App app{};

    try
    {
        app.Run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}