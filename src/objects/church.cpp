#include "objects/church.h"

#include "objects/mesh_utils.h"
#include <vector>

using MeshUtils::addTriangle;
using MeshUtils::addQuad;
using MeshUtils::vertexCount;

namespace {
    std::vector<float> vertices;

    void addNave() {
        // Frente (+Z), deixando a porta como detalhe sobreposto.
        addQuad(vertices, -3, 0, 4,  3, 0, 4,  3, 4.5f, 4, -3, 4.5f, 4);
        // Fundo (-Z).
        addQuad(vertices, 3, 0,-4, -3, 0,-4, -3, 4.5f,-4,  3, 4.5f,-4);
        // Laterais.
        addQuad(vertices, -3,0,-4, -3,0,4, -3,4.5f,4, -3,4.5f,-4);
        addQuad(vertices, 3,0, 4,  3,0,-4,  3,4.5f,-4,  3,4.5f,4);
        // Base e teto interno.
        addQuad(vertices, -3,0,-4, 3,0,-4, 3,0,4, -3,0,4);
        addQuad(vertices, -3,4.5f,4, 3,4.5f,4, 3,4.5f,-4, -3,4.5f,-4);

        // Frontão triangular da fachada.
        addTriangle(vertices, -3,4.5f,4.02f, 3,4.5f,4.02f, 0,6.5f,4.02f);
        // Frontão do fundo.
        addTriangle(vertices, 3,4.5f,-4.02f, -3,4.5f,-4.02f, 0,6.5f,-4.02f);
    }

    void addNaveRoof() {
        // Duas águas do telhado, com beiral ligeiramente saliente.
        addQuad(vertices, -3.5f,4.35f,4.4f, 0,6.6f,4.4f, 0,6.6f,-4.4f, -3.5f,4.35f,-4.4f);
        addQuad(vertices, 3.5f,4.35f,4.4f, 0,6.6f,4.4f, 0,6.6f,-4.4f,  3.5f,4.35f,-4.4f);
        // Fechamentos triangulares das extremidades do telhado.
        addTriangle(vertices, -3.5f,4.35f,4.4f, 3.5f,4.35f,4.4f, 0,6.6f,4.4f);
        addTriangle(vertices, 3.5f,4.35f,-4.4f, -3.5f,4.35f,-4.4f, 0,6.6f,-4.4f);
    }

    void addTower() {
        const float x1 = -1.05f, x2 = 1.05f;
        const float z1 = 2.9f, z2 = 4.55f;
        const float y0 = 4.4f, y1 = 7.6f;
        addQuad(vertices, x1,y0,z2, x2,y0,z2, x2,y1,z2, x1,y1,z2);
        addQuad(vertices, x2,y0,z1, x1,y0,z1, x1,y1,z1, x2,y1,z1);
        addQuad(vertices, x1,y0,z1, x1,y0,z2, x1,y1,z2, x1,y1,z1);
        addQuad(vertices, x2,y0,z2, x2,y0,z1, x2,y1,z1, x2,y1,z2);
        addQuad(vertices, x1,y1,z2, x2,y1,z2, x2,y1,z1, x1,y1,z1);

        // Telhado pontudo da torre.
        addTriangle(vertices, -1.35f,7.55f,4.8f, 1.35f,7.55f,4.8f, 0,10.0f,4.0f);
        addTriangle(vertices, 1.35f,7.55f,2.65f, -1.35f,7.55f,2.65f, 0,10.0f,3.95f);
        addTriangle(vertices, -1.35f,7.55f,2.65f, -1.35f,7.55f,4.8f, 0,10.0f,3.95f);
        addTriangle(vertices, 1.35f,7.55f,4.8f, 1.35f,7.55f,2.65f, 0,10.0f,3.95f);
    }

    void addDoor() {
        // Porta dupla escura, voltada para +Z.
        addQuad(vertices, -0.75f,0.0f,4.045f, 0.75f,0.0f,4.045f, 0.75f,2.25f,4.045f, -0.75f,2.25f,4.045f);
        // Pequeno triângulo acima da porta, como arco estilizado.
        addTriangle(vertices, -0.75f,2.25f,4.05f, 0.75f,2.25f,4.05f, 0,3.05f,4.05f);
        // Divisão central da porta.
        addQuad(vertices, -0.035f,0.0f,4.06f, 0.035f,0.0f,4.06f, 0.035f,2.25f,4.06f, -0.035f,2.25f,4.06f);
    }

    void addWindows() {
        // Rosácea frontal simples: janela alta com ponta triangular.
        addQuad(vertices, -0.55f,3.25f,4.055f, 0.55f,3.25f,4.055f, 0.55f,4.15f,4.055f, -0.55f,4.15f,4.055f);
        addTriangle(vertices, -0.55f,4.15f,4.055f, 0.55f,4.15f,4.055f, 0,4.85f,4.055f);

        // Janelas altas nas duas laterais da nave.
        for (float z : {-2.8f, 0.0f, 2.8f}) {
            // Lado esquerdo (-X).
            addQuad(vertices, -3.025f,1.9f,z-0.38f, -3.025f,1.9f,z+0.38f, -3.025f,3.35f,z+0.38f, -3.025f,3.35f,z-0.38f);
            addTriangle(vertices, -3.025f,3.35f,z-0.38f, -3.025f,3.35f,z+0.38f, -3.025f,3.85f,z);
            // Lado direito (+X).
            addQuad(vertices, 3.025f,1.9f,z+0.38f, 3.025f,1.9f,z-0.38f, 3.025f,3.35f,z-0.38f, 3.025f,3.35f,z+0.38f);
            addTriangle(vertices, 3.025f,3.35f,z+0.38f, 3.025f,3.35f,z-0.38f, 3.025f,3.85f,z);
        }

        // Abertura frontal da torre.
        addQuad(vertices, -0.42f,5.55f,4.57f, 0.42f,5.55f,4.57f, 0.42f,6.85f,4.57f, -0.42f,6.85f,4.57f);
        addTriangle(vertices, -0.42f,6.85f,4.57f, 0.42f,6.85f,4.57f, 0,7.35f,4.57f);
    }

    void addTrim() {
        // Faixas de pedra na base e nos níveis da torre.
        addQuad(vertices, -3.12f,0.25f,4.08f, 3.12f,0.25f,4.08f, 3.12f,0.38f,4.08f, -3.12f,0.38f,4.08f);
        addQuad(vertices, -1.12f,7.45f,4.62f, 1.12f,7.45f,4.62f, 1.12f,7.62f,4.62f, -1.12f,7.62f,4.62f);
        // Cruz simples no topo do telhado.
        addQuad(vertices, -0.10f,8.6f,4.02f, 0.10f,8.6f,4.02f, 0.10f,9.65f,4.02f, -0.10f,9.65f,4.02f);
        addQuad(vertices, -0.42f,9.05f,4.025f, 0.42f,9.05f,4.025f, 0.42f,9.22f,4.025f, -0.42f,9.22f,4.025f);
    }
}

Church::Church() {
    stoneStart = vertexCount(vertices);
    addNave();
    addTower();
    stoneCount = vertexCount(vertices) - stoneStart;

    roofStart = vertexCount(vertices);
    addNaveRoof();
    roofCount = vertexCount(vertices) - roofStart;

    doorStart = vertexCount(vertices);
    addDoor();
    doorCount = vertexCount(vertices) - doorStart;

    windowStart = vertexCount(vertices);
    addWindows();
    windowCount = vertexCount(vertices) - windowStart;

    trimStart = vertexCount(vertices);
    addTrim();
    trimCount = vertexCount(vertices) - trimStart;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Church::~Church() {
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
}

void Church::draw() const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, stoneStart, stoneCount);
    glDrawArrays(GL_TRIANGLES, roofStart, roofCount);
    glDrawArrays(GL_TRIANGLES, doorStart, doorCount);
    glDrawArrays(GL_TRIANGLES, windowStart, windowCount);
    glDrawArrays(GL_TRIANGLES, trimStart, trimCount);
    glBindVertexArray(0);
}

void Church::draw(GLint colorLocation) const {
    glBindVertexArray(VAO);

    glUniform3f(colorLocation, 0.36f, 0.34f, 0.36f); // pedra fria
    glDrawArrays(GL_TRIANGLES, stoneStart, stoneCount);

    glUniform3f(colorLocation, 0.16f, 0.13f, 0.19f); // telhado roxo quase preto
    glDrawArrays(GL_TRIANGLES, roofStart, roofCount);

    glUniform3f(colorLocation, 0.075f, 0.055f, 0.08f); // madeira escura
    glDrawArrays(GL_TRIANGLES, doorStart, doorCount);

    glUniform3f(colorLocation, 0.48f, 0.16f, 0.28f); // vitrais vinho
    glDrawArrays(GL_TRIANGLES, windowStart, windowCount);

    glUniform3f(colorLocation, 0.52f, 0.49f, 0.45f); // detalhes em pedra clara
    glDrawArrays(GL_TRIANGLES, trimStart, trimCount);

    glBindVertexArray(0);
}
