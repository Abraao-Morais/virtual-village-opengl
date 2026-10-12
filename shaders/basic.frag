#version 400 core

out vec4 FragColor;

in vec4 FragPosLightSpace;

uniform vec3 groundColor;
uniform sampler2D shadowMap;

float calculateShadow(vec4 lightSpacePosition)
{
    // Converter de coordenadas homogêneas
    // para coordenadas normalizadas.
    vec3 projectedCoords =
        lightSpacePosition.xyz / lightSpacePosition.w;

    // Converter de [-1, 1] para [0, 1].
    projectedCoords = projectedCoords * 0.5 + 0.5;

    // Fora do mapa de sombras: não aplicar sombra.
    if (projectedCoords.z > 1.0 ||
        projectedCoords.x < 0.0 ||
        projectedCoords.x > 1.0 ||
        projectedCoords.y < 0.0 ||
        projectedCoords.y > 1.0)
    {
        return 0.0;
    }

    float closestDepth =
        texture(shadowMap, projectedCoords.xy).r;

    float currentDepth = projectedCoords.z;

    // Pequeno ajuste para reduzir artefatos.
    float bias = 0.002;

    return currentDepth - bias > closestDepth
        ? 0.65
        : 0.0;
}

void main()
{
    float shadow = calculateShadow(FragPosLightSpace);

    vec3 color = groundColor * (1.0 - shadow);

    FragColor = vec4(color, 1.0);
}
