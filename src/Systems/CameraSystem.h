#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/CameraComponent.h"
#include <glm/glm.hpp>
#include <algorithm>

class CameraSystem : public System {
public:
    glm::vec2 cameraPosition;
    CameraSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
        RequireComponent<CameraComponent>();
    }
    void Update(int winW, int winH, int levelW, int levelH) {
        for (auto entity : GetSystemEntities()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            auto& sprite    = entity.GetComponent<SpriteComponent>();
            auto& camera    = entity.GetComponent<CameraComponent>();

            glm::vec2 target(
                transform.position.x - (winW / 2.0f - sprite.width  * transform.scale.x / 2.0f),
                transform.position.y - (winH / 2.0f - sprite.height * transform.scale.y / 2.0f));

            target.x = std::clamp(target.x, 0.0f, std::max(0.0f, (float)(levelW - winW)));
            target.y = std::clamp(target.y, 0.0f, std::max(0.0f, (float)(levelH - winH)));

            camera.position = target;
            cameraPosition  = target;
        }
    }
};
#endif
