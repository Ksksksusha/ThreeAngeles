#ifndef CAMERA_HPP_
#define CAMERA_HPP_
#include <iostream>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

namespace Vulkan
{

//-------------------------------------------------------------------------------//

class Camera 
{
    public:

        glm::mat4   getVeiwMatrix()         const { return view_matrix; }
        
        glm::mat4   getProjectionMatrix()   const { return projection_matrix; }

        void        updateViewMatrix();

        glm::vec3   getPosition() const { return position; }
        
        glm::vec3   getRotation() const { return rotation; }

        void        determineMove(GLFWwindow* const window, const float dt);

        void        determineRotate(GLFWwindow* const window, const float dt);

    private:
        
        float speed         = 30.f;

        float look_speed    = 80.f;
        
        glm::vec3 position{0.f, 0.f, 0.f};
        
        glm::vec3 rotation{0.f, 0.f, 0.f};

        glm::mat4 view_matrix{1.f};
        
        glm::mat4 projection_matrix{1.f};

        glm::vec3 direction;
        
        glm::vec3 right;
        
        glm::vec3 up;

        glm::vec3 s_world_up{ 0.f, 0.f, 1.f };
        
        glm::vec3 s_world_right{ 0.f, -1.f, 0.f };
        
        glm::vec3 s_world_forward{ 1.f, 0.f, 0.f };
};

//-------------------------------------------------------------------------------//

} // end of Vulkan namespcae

#endif