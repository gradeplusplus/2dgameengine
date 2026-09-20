#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <memory>
#include "../AssetStore/AssetStore.h"
#include <algorithm>

class RenderSystem: public System{
public:
    RenderSystem(){
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update(SDL_Renderer* renderer,std::unique_ptr<AssetStore>& AssetStore, glm::vec2 camera){
        struct RenderableEntity{
            TransformComponent transformComponent;
            SpriteComponent spriteComponent;
        };
        std::vector<RenderableEntity> renderableEntities;
        for (auto entity: GetSystemEntities()){
            RenderableEntity renderableEntity;
            renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
            renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
            renderableEntities.emplace_back(renderableEntity);
        }

        std::sort(renderableEntities.begin(),renderableEntities.end(),[](const RenderableEntity& a, const RenderableEntity& b){
            return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
        });

        for (auto entity: renderableEntities){
            const auto transform = entity.transformComponent;
            const auto sprite = entity.spriteComponent;
           
            SDL_Rect srcRect = sprite.srcRect;

            const float camX = sprite.isFixed ? 0.0f : camera.x;
            const float camY = sprite.isFixed ? 0.0f : camera.y;

            SDL_Rect desRect = {
                static_cast<int>(transform.position.x - camX),
                static_cast<int>(transform.position.y - camY),
                static_cast<int>(sprite.width * transform.scale.x),
                static_cast<int>(sprite.height * transform.scale.y)
            };
            
            SDL_RenderCopyEx(
                renderer,
                AssetStore->GetTexture(sprite.assetId),
                &srcRect,
                &desRect,
                transform.rotation,
                NULL,
                SDL_FLIP_NONE
            );

      }
    }
};
#endif
