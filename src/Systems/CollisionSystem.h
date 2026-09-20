#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H
#include "../ECS/ECS.h"
#include "../Events/CollisionEvent.h"
#include "../Events/EventBus.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Logger/Logger.h"
class CollisionSystem: public System{
public: 
    CollisionSystem(){
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    }
    void Update(){
        auto entity = GetSystemEntities();
        for (auto i = entity.begin(); i != entity.end(); i++){
            Entity a = *i; //value
            auto aTransform = a.GetComponent<TransformComponent>();
            auto aCollider = a.GetComponent<BoxColliderComponent>();

            for(auto j = i+1 ; j != entity.end(); j++){
                Entity b = *j;
                if (a == b){
                    continue;
                }
                auto bTranform = b.GetComponent<TransformComponent>();
                auto bCollider = b.GetComponent<BoxColliderComponent>();

                bool CollisionHappened = CheckAABBCollision(
                    aTransform.position.x,
                    aTransform.position.y,
                    aCollider.width,
                    aCollider.height,
                    bTranform.position.x,
                    bTranform.position.y,
                    bCollider.width,
                    bCollider.height
                );

                if (CollisionHappened){
                    EventBus::Emit<CollisionEvent>(a, b, aCollider.tag, bCollider.tag,
                    aCollider.isTrigger || bCollider.isTrigger);
                }
            }
        }
    }
    bool CheckAABBCollision(double aX, double aY, double aW, double aH,double bX, double bY,double bW, double bH){
        return (
            aX < bX + bW &&
            aX + aW > bX &&
            aY < bY + bH &&
            aY + aH > bY
        );
    }
};

#endif
