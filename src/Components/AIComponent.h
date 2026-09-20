#ifndef AICOMPONENT_H
#define AICOMPONENT_H
#include "../ECS/ECS.h"

struct AIComponent {
    Entity target;
    double speed;
    AIComponent(Entity target = Entity(), double speed = 40.0) {
        this->target = target;
        this->speed = speed;
    }
};

#endif
