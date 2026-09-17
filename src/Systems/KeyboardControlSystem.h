#ifndef KEYBOARDCONTROLSYSTEM_H
#define KEYBOARDCONTROLSYSTEM_H
#include "../ECS/ECS.h"
#include "../Components/KeyBoardControlComponent.h"
#include "../Components/RigidBodyComponent.h"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

class KeyboardControlSystem : public System{
public:
    KeyboardControlSystem(){
        RequireComponent<KeyBoardControlComponet>();
        RequireComponent<RigidBodyCompoent>();
    }
    void Update(){
        const uint8_t* state = SDL_GetKeyboardState(NULL);
        for (auto entity : GetSystemEntities()){
            const auto& keyboardControl = entity.GetComponent<KeyBoardControlComponet>();
            auto& rigidBody = entity.GetComponent<RigidBodyCompoent>();

            glm::vec2 dir(0.0f, 0.0f);

            if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
                dir.y -= 1.0f;
            }
            if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) {
                dir.y += 1.0f;
            }
            if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
                dir.x -= 1.0f;
            }
            if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
                dir.x += 1.0f;
            }

            if (glm::length(dir) > 0.0f) {
                dir = glm::normalize(dir);
            }

            rigidBody.velocity = dir * keyboardControl.speed;

        }
    } 
};
#endif

