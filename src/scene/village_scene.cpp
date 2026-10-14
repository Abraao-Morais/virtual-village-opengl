#include "scene/village_scene.h"

#include "scene/scene_transforms.h"
#include "rendering/shaders_utils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

using SceneTransforms::createModel;

VillageScene::VillageScene(): 
    modelGround(1.0f),

    houseModels{
          createModel({-9.0f, 0.0f, -4.0f}),
          createModel({-3.0f, 0.0f, -4.0f}),
          createModel({ 3.0f, 0.0f, -4.0f}),
          createModel({ 9.0f, 0.0f, -4.0f}),
          createModel({-9.0f, 0.0f, 8.0f}, 180.0f),
          createModel({-3.0f, 0.0f, 8.0f}, 180.0f),
          createModel({ 3.0f, 0.0f, 8.0f}, 180.0f),
          createModel({ 9.0f, 0.0f, 8.0f}, 180.0f)
      },

      towerModels{
          createModel({45.0f, 0.0f, -8.0f}),
          createModel({45.0f, 0.0f,  8.0f})
      },

      modelWindmill(createModel({0.0f, 0.0f, -12.0f})),
      rotorModel(1.0f),
      bladeAngle(0.0f),
      rotationSpeed(glm::radians(7.0f)),

      modelChurch(createModel({-20.0f, 0.0f, 8.0f}, 90.0f)),

      graveStonesModels{
        // Fileira 1
        createModel({-39.0f, 0.0f,  6.0f}),
        createModel({-36.5f, 0.0f,  6.0f}),
        createModel({-34.0f, 0.0f,  6.0f}),
        createModel({-31.5f, 0.0f,  6.0f}),

        // Fileira 2
        createModel({-39.0f, 0.0f,  9.0f}),
        createModel({-36.5f, 0.0f,  9.0f}),
        createModel({-34.0f, 0.0f,  9.0f}, 75.0f),
        createModel({-31.5f, 0.0f,  9.0f}),

        // Fileira 3
        createModel({-39.0f, 0.0f, 12.0f}),
        createModel({-36.5f, 0.0f, 12.0f}),
        createModel({-34.0f, 0.0f, 12.0f}),
        createModel({-31.5f, 0.0f, 12.0f}),

        // Fileira 4
        createModel({-39.0f, 0.0f, 15.0f}),
        createModel({-36.5f, 0.0f, 15.0f}),
        createModel({-34.0f, 0.0f, 15.0f}),
        createModel({-31.5f, 0.0f, 15.0f})
    },

    modelPumpkin(createModel({20.0f, 0.0f, 10.0f}, 270.0f))
{
    updateRotorModel();
}

void VillageScene::update(float deltaTime) {
    bladeAngle += rotationSpeed * deltaTime;
    updateRotorModel();

    if (gravestoneSwingActive) {
        gravestoneSwingElapsed += deltaTime;
        if (gravestoneSwingElapsed >= GRAVESTONE_SWING_DURATION) {
            gravestoneSwingElapsed = GRAVESTONE_SWING_DURATION;
            gravestoneSwingActive = false;
        }
    }
}

void VillageScene::updateRotorModel() {
    rotorModel = glm::translate( modelWindmill, glm::vec3(0.0f, 7.0f, 2.6f));
    rotorModel = glm::rotate(rotorModel, bladeAngle, glm::vec3(0.0f, 0.0f, 1.0f));
}

void VillageScene::draw(GLint modelLocation, GLint colorLocation){
    // Chao
    ShaderUtils::setMat4(modelLocation, modelGround);
    ShaderUtils::setColor(colorLocation, glm::vec3(0.3098f, 0.2352f, 0.1411f));
    ground.draw();

    // Casas
    for (const auto& model : houseModels){
        ShaderUtils::setMat4(modelLocation, model);
        house.draw(colorLocation);
    }

    // Base do moinho
    ShaderUtils::setMat4(modelLocation, modelWindmill);
    windmill.draw(colorLocation);

    // Pas animadas
    ShaderUtils::setMat4(modelLocation, rotorModel);
    windmill.drawBlades(colorLocation);

    // Torres
    for (const auto& model : towerModels){
        ShaderUtils::setMat4(modelLocation, model);
        tower.draw(colorLocation);
    }

    // Igreja
    ShaderUtils::setMat4(modelLocation, modelChurch);
    church.draw(colorLocation);

    // Lápides
    for (std::size_t i = 0; i < graveStonesModels.size(); ++i) {
        const glm::mat4 model = getGravestoneModel(i);
        ShaderUtils::setMat4(modelLocation, model);
        gravestone.draw(colorLocation);
    }

    // Abobora
    ShaderUtils::setMat4(modelLocation, modelPumpkin);
    pumpkin.draw(colorLocation);
}

void VillageScene::drawShadow(GLint modelLocation, GLint lightSpaceLocation, const glm::mat4& lightSpaceMatrix) {
    ShaderUtils::setMat4(lightSpaceLocation, lightSpaceMatrix);

    // Chao
    ShaderUtils::setMat4(modelLocation, modelGround);
    ground.draw();

    // Casas
    for (const auto& model : houseModels) {
        ShaderUtils::setMat4(modelLocation, model);
        house.draw();
    }

    // Base do moinho
    ShaderUtils::setMat4(modelLocation, modelWindmill);
    windmill.draw();

    // Pas do moinho
    ShaderUtils::setMat4(modelLocation, rotorModel);
    windmill.drawBlades();

    // Torres
    for (const auto& model : towerModels) {
        ShaderUtils::setMat4(modelLocation, model);
        tower.draw();
    }

    // Igreja
    ShaderUtils::setMat4(modelLocation, modelChurch);
    church.draw();

    // Lápides — mesmas transformações da renderização normal
    for (std::size_t i = 0; i < graveStonesModels.size(); ++i) {
        const glm::mat4 model = getGravestoneModel(i);

        ShaderUtils::setMat4(modelLocation, model);
        gravestone.draw();
    }

     // Abobora
    ShaderUtils::setMat4(modelLocation, modelPumpkin);
    pumpkin.draw();
}

void VillageScene::triggerGravestoneSwing() {
    if (gravestoneSwingActive) return;
    gravestoneSwingElapsed = 0.0f;
    gravestoneSwingActive = true;
}

glm::mat4 VillageScene::getGravestoneModel(std::size_t index) const {
    const glm::mat4& baseModel = graveStonesModels[index];

    if (index != ANIMATED_GRAVESTONE || !gravestoneSwingActive) return baseModel;

    const float t = glm::clamp(gravestoneSwingElapsed / GRAVESTONE_SWING_DURATION, 0.0f, 1.0f);
    const float envelope = std::sin(glm::pi<float>() * t);
    const float oscillation = std::sin(4.0f * glm::pi<float>() * t);
    const float angle = GRAVESTONE_MAX_ANGLE * envelope * oscillation;
    const glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

    return baseModel * rotation;
}