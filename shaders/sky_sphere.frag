#version 330 core

in vec3 direction;

out vec4 FragColor;

uniform sampler2D skyTexture;

const float PI = 3.14159265359;

void main()
{
    vec3 dir = normalize(direction);

    float u = atan(dir.z, dir.x) / (2.0 * PI) + 0.5;
    float v = asin(clamp(dir.y, -1.0, 1.0)) / PI + 0.5;

    FragColor = texture(skyTexture, vec2(u, v));
}