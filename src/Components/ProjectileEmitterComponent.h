#ifndef PROJECTILEEMITTERCOMPONENT_H
#define PROJECTILEEMITTERCOMPONENT_H
#include <glm/glm.hpp>

struct ProjectileEmitterComponent {
    glm::vec2 projectileVelocity;
    int projectileDamage;
    bool isFriendly;
    int repeatFrequency;
    int lastEmitTime;
    ProjectileEmitterComponent(glm::vec2 projectileVelocity = glm::vec2(0, 0),
                               int projectileDamage = 10, bool isFriendly = true,
                               int repeatFrequency = 200) {
        this->projectileVelocity = projectileVelocity;
        this->projectileDamage = projectileDamage;
        this->isFriendly = isFriendly;
        this->repeatFrequency = repeatFrequency;
        this->lastEmitTime = 0;
    }
};

#endif
