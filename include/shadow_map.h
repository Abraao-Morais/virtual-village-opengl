#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

#include <glad/glad.h>

class ShadowMap {
public:
    static constexpr unsigned int WIDTH = 2048;
    static constexpr unsigned int HEIGHT = 2048;

    ShadowMap();
    ~ShadowMap();

    ShadowMap(const ShadowMap&) = delete;
    ShadowMap& operator=(const ShadowMap&) = delete;

    // Inicia a renderização do mapa de profundidade.
    void bindForWriting() const;

    // Volta ao framebuffer padrão.
    void unbind() const;

    // Disponibiliza a textura de profundidade.
    void bindDepthTexture(GLuint textureUnit = 0) const;

    GLuint getDepthTexture() const;
    bool isValid() const;

private:
    GLuint depthMapFBO;
    GLuint depthMapTexture;
    bool valid;
};

#endif