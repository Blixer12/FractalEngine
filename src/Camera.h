#pragma once

// Librarys
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace FractalEngine
{
    class FractalCamera
    {
    public:
        void SetOrthographicProjection(float Left, float Right, float Top, float Bottom, float NearPlane, float FarPlane);
        void SetPerspectiveProjection(float FOV, float AspectRatio, float NearPlane, float FarPlane);

        void SetViewDirection(glm::vec3 Position, glm::vec3 Direction, glm::vec3 UP);
        void SetViewTarget(glm::vec3 Position, glm::vec3 Target, glm::vec3 UP);
        void SetViewYXZ(glm::vec3 Position, glm::vec3 Rotation);

        const glm::mat4 &GetProjection() const { return ProjectionMatrix; }
        const glm::mat4 &GetView() const { return ViewMatrix; }

    private:
        glm::mat4 ProjectionMatrix{1.0f};
        glm::mat4 ViewMatrix{1.0f};
    };
}