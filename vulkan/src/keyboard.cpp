#include "keyboard.hpp"

#include <iostream>

namespace Vulkan
{

//-------------------------------------------------------------------------------//

void Keyboard::MoveInPlainXZ(GLFWwindow* window, float dt, Object& object, Camera& camera)
{
    glm::vec3 rotate{0};

    if (glfwGetKey(window, keys.LookRight)  == GLFW_PRESS)      rotate.y += 1.f;
    
    if (glfwGetKey(window, keys.LookLeft)   == GLFW_PRESS)      rotate.y -= 1.f;
    
    if (glfwGetKey(window, keys.LookUp)     == GLFW_PRESS)      rotate.x += 1.f;
    
    if (glfwGetKey(window, keys.LookDown)   == GLFW_PRESS)      rotate.x -= 1.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
        object.transform.rotation += look_spped_ * dt * glm::normalize(rotate);

    object.transform.rotation.x = glm::clamp(object.transform.rotation.x, -1.5f, 1.5f);

    object.transform.rotation.y = glm::mod(object.transform.rotation.y, glm::two_pi<float>());

    float yaw = object.transform.rotation.y;
    
    const glm::vec3 ForwardDir{sin(yaw), 0.f, cos(yaw)};
    
    const glm::vec3 RightDir{ForwardDir.z, 0.f, -ForwardDir.x};
    
    const glm::vec3 UpDir{0.f, -1.f, 0.f};

    glm::vec3 MoveDir{0.f};

    if (glfwGetKey(window, keys.MoveForward)    == GLFW_PRESS)          MoveDir += ForwardDir;
    
    if (glfwGetKey(window, keys.MoveBackward)   == GLFW_PRESS)          MoveDir -= ForwardDir;
    
    if (glfwGetKey(window, keys.MoveRight)      == GLFW_PRESS)          MoveDir += RightDir;
    
    if (glfwGetKey(window, keys.MoveLeft)       == GLFW_PRESS)          MoveDir -= RightDir;
    
    if (glfwGetKey(window, keys.MoveUp)         == GLFW_PRESS)          MoveDir += UpDir;
    
    if (glfwGetKey(window, keys.MoveDown)       == GLFW_PRESS)          MoveDir -= UpDir;

    if (glfwGetKey(window, keys.SpeedUp)        == GLFW_PRESS)          move_speed_ += 1;

    if (glfwGetKey(window, keys.SpeedDown)      == GLFW_PRESS)          move_speed_ -= 1;

    if (move_speed_ <= 0)
        move_speed_ = 0;

    if (glm::dot(MoveDir, MoveDir) > std::numeric_limits<float>::epsilon())
        object.transform.translation += move_speed_ * dt * glm::normalize(MoveDir);

    camera.UpdatePositionAndRotation(MoveDir, rotate, ForwardDir, RightDir, UpDir);
}

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace