#ifndef OBJECT_HPP_
#define OBJECT_HPP_

#include "model.hpp"

#include <memory>
#include <glm/gtc/matrix_transform.hpp>

namespace Vulkan 
{

struct TransformComponent {
    glm::vec3 translation{};
    glm::vec3 scale{1.f, 1.f, 1.f};
    glm::vec3 rotation{};

    glm::mat4 mat4()
    {
        auto transform = glm::translate(glm::mat4(1.f), translation);

        transform = glm::rotate(transform, rotation.y, {1.f, 0.f, 0.f});

        transform = glm::rotate(transform, rotation.x, {0.f, 0.f, 1.f});

        transform = glm::rotate(transform, rotation.z, {1.f, 0.f, 0.f});

        transform = glm::scale(transform, scale);

        return transform;
    }

    glm::mat3 normalMatrix() 
    {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        
        const glm::vec3 invScale = 1.0f / scale;

        return glm::mat3{
            {
                invScale.x * (c1 * c3 + s1 * s2 * s3),
                invScale.x * (c2 * s3),
                invScale.x * (c1 * s2 * s3 - c3 * s1),
            },
            {
                invScale.y * (c3 * s1 * s2 - c1 * s3),
                invScale.y * (c2 * c3),
                invScale.y * (c1 * c3 * s2 + s1 * s3),
            },
            {
                invScale.z * (c2 * s1),
                invScale.z * (-s2),
                invScale.z * (c1 * c2),
            },
        };
    }
};

class Object
{
    using id_t = unsigned int;

    private:

        id_t id;

        Object(id_t obj_id) : id{obj_id} {}

    public:

        static Object CreateObject()
        {
            static id_t current_id = 0;

            return Object(current_id++);
        }

        id_t GetId() const { return id; }

        std::unique_ptr<Model> model{};
        
        glm::vec3 color{};
        
        TransformComponent transform{};
};

} // end of vulkan namespace

#endif