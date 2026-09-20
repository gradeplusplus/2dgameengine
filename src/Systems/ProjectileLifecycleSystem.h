#ifndef PROJECTILELIFECYCLESYSTEM_H
#define PROJECTILELIFECYCLESYSTEM_H
#include "../ECS/ECS.h"
#include "../Components/ProjectileComponent.h"
#include <SDL2/SDL.h>

class ProjectileLifecycleSystem : public System {
public:
    ProjectileLifecycleSystem() {
        RequireComponent<ProjectileComponent>();
    }

    void Update() {
        for (auto entity : GetSystemEntities()) {
            const auto projectile = entity.GetComponent<ProjectileComponent>();
            if (static_cast<int>(SDL_GetTicks()) - projectile.startTime > projectile.duration) {
                entity.Kill();
            }
        }
    }
};

#endif
