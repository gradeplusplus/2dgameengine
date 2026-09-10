#ifndef RENDERCOLLIDERSYSTEM_H
#define RENDERCOLLIDERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

class RenderColliderSystem:public System{
public:
    RenderColliderSystem(){
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    }
    void Update(SDL_Renderer* renderer){
        for(auto entity: GetSystemEntities()){
            const auto tran = entity.GetComponent<TransformComponent>();
            const auto collider = entity.GetComponent<BoxColliderComponent>();

            SDL_Rect colliderRect = {
                static_cast<int>(tran.position.x + collider.offest.x),
                static_cast<int>(tran.position.y + collider.offest.y),
                static_cast<int>(collider.width),
                static_cast<int>(collider.height)
            };
            SDL_SetRenderDrawColor(renderer,255,0,0,255);
            SDL_RenderDrawRect(renderer, &colliderRect);
        }
    }
};
#endif
