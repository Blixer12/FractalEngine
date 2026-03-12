#pragma once

#include "Model.h"

// Standard Library
#include <memory>

namespace FractalEngine
{

    struct Transform2DComponet
    {
        glm::vec2 Translation{}; // Position
        glm::vec2 Scale{1, 1};
        float Rotation;

        glm::mat2 TransformMatrix()
        {
            const float Sine = glm::sin(Rotation);
            const float Cosine = glm::cos(Rotation);
            glm::mat2 RotationMat{{Cosine, Sine}, {-Sine, Cosine}};

            glm::mat2 ScaleMat{{Scale.x, 0}, {0, Scale.y}};
            return RotationMat * ScaleMat;
        }
    };

    class FractalGameObject
    {
    public:
        using id_t = unsigned int;

        static FractalGameObject CreateGameObject()
        {
            static id_t CurrentID = 0;
            return FractalGameObject{CurrentID++};
        }

        FractalGameObject(const FractalGameObject &) = delete;
        FractalGameObject &operator=(const FractalGameObject &) = delete;
        FractalGameObject(FractalGameObject &&) = default;
        FractalGameObject &operator=(FractalGameObject &&) = default;

        const id_t GetID() { return id; }

        std::shared_ptr<FractalModel> Model{};
        glm::vec4 Color{};
        Transform2DComponet Transform2D{};

    private:
        FractalGameObject(id_t ObjectID) : id{ObjectID} {}
        id_t id;
    };
}