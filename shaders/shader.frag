// #version 450

// layout(location = 0) in vec3 fragColor;
// layout(location = 1) in vec3 normal;
// layout(location = 2) in vec3 fragPos;
// layout(location = 3) in vec3 viewPos;

// layout(location = 0) out vec4 outColor;

// void main() 
// {
//     vec3 lightColor         = vec3(1.0, 1.0, 1.0);

//     vec3 amb_color          = vec3(0.05, 0.0, 0.8);
    
//     if (fragColor == vec3(1.0, 0.0, 0.0))
//         amb_color = vec3(0.8, 0.0, 0.2);    
    
//     float ambient_strength  = 0.2;
//     vec3 ambient            = amb_color * ambient_strength;
//     vec3 lightDir           = normalize(viewPos - fragPos);

//     float diffuse_strength  = 1.0;
//     float diff              = max(dot(normal, lightDir), 0.0);
//     vec3 diffuse            = diffuse_strength * diff * lightColor;

//     vec3 viewDir            = normalize(viewPos - fragPos);
//     vec3 reflectDir         = reflect(-lightDir, normal);

//     float specularStrength  = 1.0;
//     float spec              = pow(max(dot(viewDir, reflectDir), 0.0), 100);
//     vec3 specular           = specularStrength * spec * lightColor;

//     vec3 result             = fragColor * diffuse + specular + ambient;
    
//     vec3 dir                = viewPos - fragPos;
//     result                  = clamp(0.8 * result, result, result / dot(dir, dir) * 1000);

//     outColor                = vec4(result, 1.0);
// }

#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec3 viewPos;

layout(location = 0) out vec4 outColor;

void main() 
{
    vec3 lightPos   = vec3(50.0, 50.0, 50.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    float ambient_strength  = 1.0;
    vec3 ambient            = lightColor * ambient_strength;
    vec3 lightDir           = normalize(lightPos - fragPos);

    float diffuse_strength  = 1.0;
    float diff              = max(dot(normal, lightDir), 0.0);
    vec3 diffuse            = diffuse_strength * diff * lightColor;

    vec3 viewDir    = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float specularStrength  = 1.0;
    float spec              = pow(max(dot(viewDir, reflectDir), 0.0), 100);
    vec3 specular           = specularStrength * spec * lightColor;

    vec3 result = fragColor * (ambient + diffuse + specular);
    
    vec3 dir    = viewPos - fragPos;
    result      = clamp(0.8 * result, result, result / dot(dir, dir) * 1000);

    outColor = vec4(result, 1.0);
}