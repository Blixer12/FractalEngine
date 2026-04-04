#pragma once

#include "Model.h"

// Libraries
#include <glm/gtc/matrix_transform.hpp>

// Standard Library
#include <memory>
#include <unordered_map>

namespace FractalEngine
{

    struct TransformComponent
    {
        glm::vec3 Translation{}; // Position
        glm::vec3 Scale{1, 1, 1};
        glm::vec3 Rotation;

        glm::mat4 TransformMatrix();
        glm::mat4 NormalMatrix();
    };

    class FractalGameObject
    {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, FractalGameObject>;

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
        TransformComponent Transform{};

    private:
        FractalGameObject(id_t ObjectID) : id{ObjectID} {}
        id_t id;
    };
}