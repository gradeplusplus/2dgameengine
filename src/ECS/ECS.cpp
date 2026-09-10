#include "ECS.h"
#include "../Logger/Logger.h"
#include <algorithm>

int IComponent::nextId = 0;

int Entity::GetId() const { return id; }
void System::AddEntityToSystem(Entity entity) { entities.push_back(entity); }
void System::RemoveEntityFromSystem(Entity entity) {
    entities.erase(
        std::remove_if(entities.begin(), entities.end(),
                       [&entity](Entity other) { return entity == other; }),
        entities.end());
}
std::vector<Entity> System::GetSystemEntities() const { return entities; }
const Signature &System::GetComponentSignature() const {
    return ComponentSignature;
}

Entity Registy::CreateEntity() {
    int entityId;
    if (freeIds.empty()){
        entityId = numEntities++;
        if (entityId >= static_cast<int>(entityComponentSignatures.size())){
        entityComponentSignatures.resize(entityId + 1);
        }
    }
    else{
        entityId = freeIds.front();
        freeIds.pop_front();
    }
    Entity entity(entityId);
    //what this line doing?
    entity.registry = this;
    entitiesToBeAdded.insert(entity);
    Logger::Log("Entity created with Id = " + std::to_string(entityId));
    return entity;
}
void Registy::KillEntity(Entity entity){
    entitiesToBeKilled.insert(entity);
}
void Registy::RemoveEntityFromSystems(Entity entity){
    for (auto system: systems){
        system.second->RemoveEntityFromSystem(entity);
    }
}
void Registy::Update() {
    // add or remove the entities
    for (auto entity: entitiesToBeAdded){
        AddEntityToSystem(entity);
    }
    entitiesToBeAdded.clear();

    for (auto entity: entitiesToBeKilled){
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entity.GetId()].reset();
        freeIds.push_back(entity.GetId());
    }
    entitiesToBeKilled.clear();
}

void Registy::AddEntityToSystem(Entity entity){
    const auto entityId = entity.GetId();
    //why here is &
    const auto& entityComponentSignature = entityComponentSignatures[entityId];
    //why here use : ?
    for (auto& system: systems){
        //here system.second is a pointer?
        const auto& systemComponentSignature = system.second->GetComponentSignature();
        //here?
        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

        if (isInterested){
            system.second->AddEntityToSystem(entity);
        }
    } 
}

void Entity::Kill(){
    registry->KillEntity(*this);
}
