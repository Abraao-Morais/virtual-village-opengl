#include "scene/village_scene.h"

#include "scene/scene_transforms.h"
#include "rendering/shaders_utils.h"

#include <glm/gtc/matrix_transform.hpp>

using SceneTransforms::createModel;

VillageScene::VillageScene(): 
    modelGround(1.0f),

    houseModels{
          createModel({-9.0f, 0.0f, -4.0f}),
          createModel({-3.0f, 0.0f, -4.0f}),
          createModel({ 3.0f, 0.0f, -4.0f}),
          createModel({ 9.0f, 0.0f, -4.0f}),
          createModel({-9.0f, 0.0f, 4.0f}, 180.0f),
          createModel({-3.0f, 0.0f, 4.0f}, 180.0f),
          createModel({ 3.0f, 0.0f, 4.0f}, 180.0f),
          createModel({ 9.0f, 0.0f, 4.0f}, 180.0f)
      },

      towerModels{
          createModel({50.0f, 0.0f, -8.0f}),
          createModel({50.0f, 0.0f,  8.0f})
      },

      modelWindmill(createModel({0.0f, 0.0f, -12.0f})),
      rotorModel(1.0f),
      bladeAngle(0.0f),
      rotationSpeed(glm::radians(7.0f)) 
{
    updateRotorModel();
}

void VillageScene::update(float deltaTime) {
    bladeAngle += rotationSpeed * deltaTime;
    updateRotorModel();
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
}