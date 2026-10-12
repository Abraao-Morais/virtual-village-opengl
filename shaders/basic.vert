#version 400 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec4 FragPosLightSpace;

void main()
{
    vec4 worldPosition = model * vec4(vertexPosition, 1.0);

    FragPosLightSpace = lightSpaceMatrix * worldPosition;

    gl_Position = projection * view * worldPosition;
}
