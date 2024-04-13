#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Vulkan
{

//-------------------------------------------------------------------------------//

class Camera
{
    public:

        void SetOrthographProjection(float left, float right, float top, float bottom, float near, float far)
        {
            projection_matrix_ = glm::ortho(left, right, top, bottom, near, far);
        }

        void SetPerspectiveProjection(float fovy, float aspect, float near, float far)
        {
            projection_matrix_ = glm::perspectiveLH_ZO(fovy, aspect, near, far);
        }

        void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3(0.f, -1.f, 0.f))
        {
            view_matrix_ = glm::lookAt(position, direction, up);
        }

        void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3(0.f, -1.f, 0.f))
        {
            SetViewDirection(position, target - position, up);
        }

        void SetViewYXZ(glm::vec3 position, glm::vec3 rotation)
        {
            const float c3 = glm::cos(rotation.z);
            const float s3 = glm::sin(rotation.z);
            const float c2 = glm::cos(rotation.x);
            const float s2 = glm::sin(rotation.x);
            const float c1 = glm::cos(rotation.y);
            const float s1 = glm::sin(rotation.y);
            
            const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
            const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
            const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
            
            view_matrix_ = glm::mat4{1.f};

            view_matrix_ = glm::rotate(view_matrix_, rotation.y, v);
            view_matrix_ = glm::rotate(view_matrix_, rotation.x, u);
            view_matrix_ = glm::rotate(view_matrix_, rotation.z, w);
            view_matrix_ = glm::transpose(view_matrix_);
            view_matrix_ = glm::translate(view_matrix_, -position);
        }

        const glm::mat4& GetProjectionMatrix()  const { return projection_matrix_; }

        const glm::mat4& GetView()              const { return view_matrix_; }
    
        const glm::vec3& GetPosition()          const { return position_; }
        
        const glm::vec3& GetRotation()          const { return rotation_; }

        void UpdatePositionAndRotation(const glm::vec3 movement_delta, const glm::vec3 rotation_delta,
                                    const glm::vec3 dir, const glm::vec3 right, const glm::vec3 up)
        {
            position_ += dir        * movement_delta.x;

            position_ += right      * movement_delta.y;

            position_ += up         * movement_delta.z;

            rotation_ += rotation_delta;
        }

    private:

        glm::vec3 position_{0};
        
        glm::vec3 rotation_{0};

        glm::mat4 view_matrix_;
        
        glm::mat4 projection_matrix_;
};

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace

#endif