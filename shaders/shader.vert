#version 450

layout(binding = 0) uniform Ubo {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 inViewPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 fragPos;
layout(location = 3) out vec3 viewPos;

void main () 
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    
    fragColor   = inColor;
    
    normal      = normalize(mat3(ubo.model) * inNormal);
    
    fragPos     = vec3(ubo.model * vec4(inPosition, 1.0));
    
    viewPos     = ubo.inViewPos;
}