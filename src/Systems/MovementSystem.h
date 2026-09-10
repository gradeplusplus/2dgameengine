#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H
#include "../ECS/ECS.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"

class MovementSystem : public System {
public:
  MovementSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyCompoent>();
    }
  void Update(double deltaTime) {
    // loop all entites that the system is intrested intrested
        for (auto entity: GetSystemEntities()){
            TransformComponent& transform = entity.GetComponent<TransformComponent>();
            const RigidBodyCompoent rigidbody = entity.GetComponent<RigidBodyCompoent>();

            transform.position.x += rigidbody.velocity.x * deltaTime;
            transform.position.y += rigidbody.velocity.y * deltaTime;
        }
  }
};

#endif
