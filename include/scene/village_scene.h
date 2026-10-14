#pragma once

#include <array>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "objects/ground.h"
#include "objects/house.h"
#include "objects/windmill.h"
#include "objects/tower.h"

class VillageScene {
    public:
        VillageScene();

        void update(float deltaTime);

        void draw(GLint modelLocation, GLint colorLocation);

        void drawShadow(
            GLint modelLocation,
            GLint lightSpaceLocation,
            const glm::mat4& lightSpaceMatrix
        );

    private:
        Ground ground;
        House house;
        Windmill windmill;
        Tower tower;

        glm::mat4 modelGround;
        std::array<glm::mat4, 8> houseModels;

        glm::mat4 modelWindmill;
        glm::mat4 rotorModel;

        std::array<glm::mat4, 2> towerModels;

        float bladeAngle;
        float rotationSpeed;

        void updateRotorModel();
};