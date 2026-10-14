#pragma once

#include <array>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "objects/ground.h"
#include "objects/house.h"
#include "objects/windmill.h"
#include "objects/tower.h"
#include "objects/church.h"
#include "objects/gravestone.h"
#include "objects/pumpkin.h"

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
        glm::mat4 modelGround;

        House house;
        std::array<glm::mat4, 8> houseModels;

        Windmill windmill;
        glm::mat4 modelWindmill;
        glm::mat4 rotorModel;
        float bladeAngle;
        float rotationSpeed;
        void updateRotorModel();

        Tower tower;
        std::array<glm::mat4, 2> towerModels;

        Church church;
        glm::mat4 modelChurch;

        Gravestone gravestone;
        std::array<glm::mat4, 16> graveStonesModels;

        Pumpkin pumpkin;
        glm::mat4 modelPumpkin;
};