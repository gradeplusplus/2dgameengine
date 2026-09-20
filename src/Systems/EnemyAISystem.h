#ifndef ENEMYAISYSTEM_H
#define ENEMYAISYSTEM_H
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AIComponent.h"
#include <glm/glm.hpp>

class EnemyAISystem : public System {
public:
    EnemyAISystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyCompoent>();
        RequireComponent<AIComponent>();
    }

    void Update() {
        for (auto entity : GetSystemEntities()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            auto& rigidBody = entity.GetComponent<RigidBodyCompoent>();
            const auto ai = entity.GetComponent<AIComponent>();

            auto& targetTransform = ai.target.GetComponent<TransformComponent>();
            glm::vec2 dir = targetTransform.position - transform.position;

            if (glm::length(dir) > 0.0f) {
                dir = glm::normalize(dir);
                rigidBody.velocity = dir * static_cast<float>(ai.speed);
            } else {
                rigidBody.velocity = glm::vec2(0.0f);
            }
        }
    }
};

#endif
