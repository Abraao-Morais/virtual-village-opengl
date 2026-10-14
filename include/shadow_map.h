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

        void bindForWriting() const;

        void unbind() const;

        void bindDepthTexture(GLuint textureUnit = 0) const;

        GLuint getDepthTexture() const;
        bool isValid() const;

    private:
        GLuint depthMapFBO;
        GLuint depthMapTexture;
        bool valid;
};

#endif