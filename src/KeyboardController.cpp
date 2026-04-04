#include "KeyboardController.h"

namespace FractalEngine
{
    void KeyboardController::MoveInPlaneXZ(GLFWwindow *Window, float DeltaTime, FractalGameObject &GameObject)
    {
        glm::vec3 Rotate{0};
        if (glfwGetKey(Window, Keys.LookRight) == GLFW_PRESS)
            Rotate.y += 1.f;
        if (glfwGetKey(Window, Keys.LookLeft) == GLFW_PRESS)
            Rotate.y -= 1.f;
        if (glfwGetKey(Window, Keys.LookUp) == GLFW_PRESS)
            Rotate.x += 1.f;
        if (glfwGetKey(Window, Keys.LookDown) == GLFW_PRESS)
            Rotate.x -= 1.f;
        if (glm::dot(Rotate, Rotate) > std::numeric_limits<float>::epsilon())
        {
            GameObject.Transform.Rotation += glm::normalize(Rotate) * TurnSpeed * DeltaTime;
        }

        // Limit Rotation to a certain range to avoid flipping
        GameObject.Transform.Rotation.x = glm::clamp(GameObject.Transform.Rotation.x, -1.5f, 1.5f);
        GameObject.Transform.Rotation.y = glm::mod(GameObject.Transform.Rotation.y, glm::two_pi<float>());

        float Yaw = GameObject.Transform.Rotation.y;
        const glm::vec3 ForwardDir{glm::sin(Yaw), 0.f, glm::cos(Yaw)};
        const glm::vec3 RightDir{ForwardDir.z, 0.f, -ForwardDir.x};
        const glm::vec3 UpDir{0.f, -1.f, 0.f};

        glm::vec3 MoveDir{0.f};
        if (glfwGetKey(Window, Keys.MoveForward) == GLFW_PRESS)
            MoveDir += ForwardDir;
        if (glfwGetKey(Window, Keys.MoveBackward) == GLFW_PRESS)
            MoveDir -= ForwardDir;
        if (glfwGetKey(Window, Keys.MoveRight) == GLFW_PRESS)
            MoveDir += RightDir;
        if (glfwGetKey(Window, Keys.MoveLeft) == GLFW_PRESS)
            MoveDir -= RightDir;
        if (glfwGetKey(Window, Keys.MoveUp) == GLFW_PRESS)
            MoveDir += UpDir;
        if (glfwGetKey(Window, Keys.MoveDown) == GLFW_PRESS)
            MoveDir -= UpDir;

        if (glm::dot(MoveDir, MoveDir) > std::numeric_limits<float>::epsilon())
        {
            GameObject.Transform.Translation += glm::normalize(MoveDir) * MoveSpeed * DeltaTime;
        }
    }
}