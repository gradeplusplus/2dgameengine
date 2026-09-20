#ifndef SCRIPTINGSYSTEM_H
#define SCRIPTINGSYSTEM_H
#include "../ECS/ECS.h"
#include "../Components/ScriptComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Logger/Logger.h"
#include <sol/sol.hpp>
#include <string>
#include <unordered_map>

class ScriptingSystem : public System {
public:
    sol::state lua;
    std::unordered_map<int, sol::protected_function> onUpdateFunctions;

    ScriptingSystem() {
        RequireComponent<ScriptComponent>();
        lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
        RegisterTypes();
    }

    void RegisterTypes() {
        lua.new_usertype<TransformComponent>("Transform",
            "get_position_x", [](TransformComponent& t) { return t.position.x; },
            "get_position_y", [](TransformComponent& t) { return t.position.y; },
            "set_position", [](TransformComponent& t, float x, float y) {
                t.position.x = x;
                t.position.y = y;
            },
            "get_rotation", [](TransformComponent& t) { return t.rotation; },
            "set_rotation", [](TransformComponent& t, double r) { t.rotation = r; });

        lua.new_usertype<RigidBodyCompoent>("RigidBody",
            "get_velocity_x", [](RigidBodyCompoent& r) { return r.velocity.x; },
            "get_velocity_y", [](RigidBodyCompoent& r) { return r.velocity.y; },
            "set_velocity", [](RigidBodyCompoent& r, float x, float y) {
                r.velocity.x = x;
                r.velocity.y = y;
            });

        lua.new_usertype<Entity>("Entity",
            "get_id", &Entity::GetId,
            "kill", &Entity::Kill,
            "get_transform", &Entity::GetComponent<TransformComponent>,
            "get_rigidbody", &Entity::GetComponent<RigidBodyCompoent>);
    }

    void Update(double deltaTime) {
        for (auto entity : GetSystemEntities()) {
            const auto script = entity.GetComponent<ScriptComponent>();
            if (script.scriptPath.empty()) {
                continue;
            }

            const int entityId = entity.GetId();
            if (onUpdateFunctions.find(entityId) == onUpdateFunctions.end()) {
                try {
                    lua.script_file(script.scriptPath);
                } catch (const sol::error& e) {
                    Logger::Err("Script error in " + script.scriptPath + ": " + e.what());
                    continue;
                }

                sol::object onUpdateObject = lua["on_update"];
                if (!onUpdateObject.is<sol::protected_function>()) {
                    Logger::Err("Script " + script.scriptPath + " has no on_update function");
                    continue;
                }
                onUpdateFunctions.emplace(entityId, onUpdateObject.as<sol::protected_function>());
            }

            sol::protected_function_result result = onUpdateFunctions[entityId](entity, deltaTime);
            if (!result.valid()) {
                sol::error err = result;
                Logger::Err("Script runtime error in " + script.scriptPath + ": " + err.what());
            }
        }
    }
};

#endif
