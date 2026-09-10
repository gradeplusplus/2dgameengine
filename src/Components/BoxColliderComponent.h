#ifndef BOXCOLLIDERCOMPONENT_H
#define BOXCOLLIDERCOMPONENT_H
#include <glm/glm.hpp>
struct BoxColliderComponent{
    int width;
    int height;
    glm::vec2 offest;
    BoxColliderComponent(int width = 0, int height = 0, glm::vec2 offest = glm::vec2(0,0)){
        this->width = width;
        this->height = height;
        this->offest = offest;
    }
};
#endif
