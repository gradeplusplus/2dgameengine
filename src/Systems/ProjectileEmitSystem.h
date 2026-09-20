#ifndef PROJECTILEEMITSYSTEM_H
#define PROJECTILEEMITSYSTEM_H
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

class ProjectileEmitSystem : public System {
public:
    ProjectileEmitSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<ProjectileEmitterComponent>();
    }

    void Update() {
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (!state[SDL_SCANCODE_SPACE]) {
            return;
        }

        for (auto entity : GetSystemEntities()) {
            const auto transform = entity.GetComponent<TransformComponent>();
            const auto emitter = entity.GetComponent<ProjectileEmitterComponent>();

            if (static_cast<int>(SDL_GetTicks()) - emitter.lastEmitTime < emitter.repeatFrequency) {
                continue;
            }
            entity.GetComponent<ProjectileEmitterComponent>().lastEmitTime = SDL_GetTicks();

            Entity projectile = registry->CreateEntity();
            projectile.AddComponent<TransformComponent>(transform.position, glm::vec2(1.0, 1.0), 0.0);
            projectile.AddComponent<RigidBodyCompoent>(emitter.projectileVelocity);
            projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 3);
            projectile.AddComponent<BoxColliderComponent>(4, 4, glm::vec2(0, 0), "projectile", true);
            projectile.AddComponent<ProjectileComponent>(emitter.projectileDamage, emitter.isFriendly);
        }
    }
};

#endif
