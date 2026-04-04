#include "Camera.h"

// Standard Library
#include <cassert>
#include <limits>

namespace FractalEngine
{
    void FractalCamera::SetOrthographicProjection(float Left, float Right, float Top, float Bottom, float NearPlane, float FarPlane)
    {
        ProjectionMatrix = glm::mat4{1.0f};
        ProjectionMatrix[0][0] = 2.f / (Right - Left);
        ProjectionMatrix[1][1] = 2.f / (Bottom - Top);
        ProjectionMatrix[2][2] = 1.f / (FarPlane - NearPlane);
        ProjectionMatrix[3][0] = -(Right + Left) / (Right - Left);
        ProjectionMatrix[3][1] = -(Bottom + Top) / (Bottom - Top);
        ProjectionMatrix[3][2] = -NearPlane / (FarPlane - NearPlane);
    }

    void FractalCamera::SetPerspectiveProjection(float FOV, float AspectRatio, float NearPlane, float FarPlane)
    {
        assert(glm::abs(AspectRatio - std::numeric_limits<float>::epsilon()) > 0.0f);
        const float tanHalfFovy = tan(FOV / 2.f);
        ProjectionMatrix = glm::mat4{0.0f};
        ProjectionMatrix[0][0] = 1.f / (AspectRatio * tanHalfFovy);
        ProjectionMatrix[1][1] = 1.f / (tanHalfFovy);
        ProjectionMatrix[2][2] = FarPlane / (FarPlane - NearPlane);
        ProjectionMatrix[2][3] = 1.f;
        ProjectionMatrix[3][2] = -(FarPlane * NearPlane) / (FarPlane - NearPlane);
    }

    void FractalCamera::SetViewDirection(glm::vec3 Position, glm::vec3 Direction, glm::vec3 UP)
    {
        const glm::vec3 w{glm::normalize(Direction)};
        const glm::vec3 u{glm::normalize(glm::cross(w, UP))};
        const glm::vec3 v{glm::cross(w, u)};

        ViewMatrix = glm::mat4{1.f};
        ViewMatrix[0][0] = u.x;
        ViewMatrix[1][0] = u.y;
        ViewMatrix[2][0] = u.z;
        ViewMatrix[0][1] = v.x;
        ViewMatrix[1][1] = v.y;
        ViewMatrix[2][1] = v.z;
        ViewMatrix[0][2] = w.x;
        ViewMatrix[1][2] = w.y;
        ViewMatrix[2][2] = w.z;
        ViewMatrix[3][0] = -glm::dot(u, Position);
        ViewMatrix[3][1] = -glm::dot(v, Position);
        ViewMatrix[3][2] = -glm::dot(w, Position);
    }

    void FractalCamera::SetViewTarget(glm::vec3 Position, glm::vec3 Target, glm::vec3 UP)
    {
        SetViewDirection(Position, Target - Position, UP);
    }

    void FractalCamera::SetViewYXZ(glm::vec3 Position, glm::vec3 Rotation)
    {
        const float c3 = glm::cos(Rotation.z);
        const float s3 = glm::sin(Rotation.z);
        const float c2 = glm::cos(Rotation.x);
        const float s2 = glm::sin(Rotation.x);
        const float c1 = glm::cos(Rotation.y);
        const float s1 = glm::sin(Rotation.y);
        const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
        const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
        const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
        ViewMatrix = glm::mat4{1.f};
        ViewMatrix[0][0] = u.x;
        ViewMatrix[1][0] = u.y;
        ViewMatrix[2][0] = u.z;
        ViewMatrix[0][1] = v.x;
        ViewMatrix[1][1] = v.y;
        ViewMatrix[2][1] = v.z;
        ViewMatrix[0][2] = w.x;
        ViewMatrix[1][2] = w.y;
        ViewMatrix[2][2] = w.z;
        ViewMatrix[3][0] = -glm::dot(u, Position);
        ViewMatrix[3][1] = -glm::dot(v, Position);
        ViewMatrix[3][2] = -glm::dot(w, Position);
    }
}