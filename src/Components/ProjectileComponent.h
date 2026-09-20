#ifndef PROJECTILECOMPONENT_H
#define PROJECTILECOMPONENT_H
#include <SDL2/SDL.h>

struct ProjectileComponent {
    int damage;
    bool isFriendly;
    int duration;
    int startTime;
    ProjectileComponent(int damage = 10, bool isFriendly = true, int duration = 2000) {
        this->damage = damage;
        this->isFriendly = isFriendly;
        this->duration = duration;
        this->startTime = SDL_GetTicks();
    }
};

#endif
