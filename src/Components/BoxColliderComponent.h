#ifndef BOXCOLLIDERCOMPONENT_H
#define BOXCOLLIDERCOMPONENT_H
#include <string>
#include <glm/glm.hpp>
#include <string>
struct BoxColliderComponent{
    int width;
    int height;
    glm::vec2 offest;
    std::string tag;
    bool isTrigger;
    BoxColliderComponent(int width = 0, int height = 0, glm::vec2 offest = glm::vec2(0,0),
                         std::string tag = "", bool isTrigger = false){
        this->width = width;
        this->height = height;
        this->offest = offest;
        this->tag = tag;
        this->isTrigger = isTrigger;
    }
};
#endif
