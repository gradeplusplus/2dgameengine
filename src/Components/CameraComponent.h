#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H
#include <glm/glm.hpp>
struct CameraComponent {
    glm::vec2 position;
    CameraComponent(glm::vec2 position = glm::vec2(0,0)) { this->position = position; }
};
#endif
