#include "rendering/shadow_map.h"

#include <iostream>

ShadowMap::ShadowMap(): depthMapFBO(0), depthMapTexture(0), valid(false) {
    // Criar o framebuffer
    glGenFramebuffers(1, &depthMapFBO);

    // Criar a textura de profundidade
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Configurar a filtragem da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Evitar artefatos nas bordas do mapa
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    const float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,borderColor);

    // Associar a textura ao framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    // Este framebuffer só armazena profundidade.
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Verificar se o framebuffer está completo
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        valid = true;
    else
        std::cerr << "Erro: framebuffer do ShadowMap incompleto.\n";

    // Restaurar o estado padrão
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

ShadowMap::~ShadowMap() {
    if (depthMapTexture != 0)
        glDeleteTextures(1, &depthMapTexture);

    if (depthMapFBO != 0)
        glDeleteFramebuffers(1, &depthMapFBO);
}

void ShadowMap::bindForWriting() const {
    if (!valid)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glViewport(0, 0, WIDTH, HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::bindDepthTexture(GLuint textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
}

GLuint ShadowMap::getDepthTexture() const {
    return depthMapTexture;
}

bool ShadowMap::isValid() const {
    return valid;
}