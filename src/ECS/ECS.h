#ifndef ECS_H
#define ECS_H
#include <deque>
#include <bitset>
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

const unsigned int MAX_COMPONENTS = 32;
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
protected:
    static int nextId;
};
template <typename T> class Component : public IComponent {
public:
    static int GetId() {
        static auto id = nextId++;
        return id;
    }
};
class Entity {
private:
    int id;

public:
    Entity() : id(-1) {};
    Entity(int id) : id(id) {};
    Entity(const Entity &entity) = default;
    int GetId() const;
    Entity &operator=(const Entity &other) = default;
    bool operator==(const Entity &other) const { return id == other.id; }
    bool operator!=(const Entity &other) const { return id != other.id; }
    bool operator>(const Entity &other) const { return id > other.id; }
    bool operator<(const Entity &other) const { return id < other.id; }
    void Kill(); 
    template <typename TComponent,typename ...TArgs> void AddComponent(TArgs&& ...args);
    template<typename TComponent> void RemoveComponent();
    template<typename TComponent> bool HasComponent() const;
    template<typename TComponent> TComponent& GetComponent() const;
   
    //why use this?
    class Registy* registry = nullptr;
};
class System {
private:
    Signature ComponentSignature;
    std::vector<Entity> entities;

public:
    System() = default;
    virtual ~System() = default;

    class Registy* registry = nullptr;

    void AddEntityToSystem(Entity entity);
    void RemoveEntityFromSystem(Entity entity);
    std::vector<Entity> GetSystemEntities() const;
    const Signature &GetComponentSignature() const;
    template <typename TComponent> void RequireComponent();
};
class IPool {
public:
    virtual ~IPool() {}
};
// vector of objects of type T
template <typename T> class Pool : public IPool {
private:
    std::vector<T> data;

public:
    Pool(int size = 100) { data.resize(size); }
    virtual ~Pool() = default;
    bool isEmpty() const { return data.empty(); }
    int getSize() const { return data.size(); }
    void Resize(int n) { data.resize(n); }
    void Clear() { data.clear(); }
    void Add(T object) { data.push_back(object); }
    void Set(int index, T object) { data[index] = object; }
    T& Get(int index) { return static_cast<T &>(data[index]); }
    T& operator[](unsigned int index) { return data[index]; }
};
// registry manger entities, add systems and components.
class Registy {
private:
    int numEntities = 0;
    // vector id is component id , pool id is entity id.
    std::vector<std::shared_ptr<IPool>> componentPools;
    // vector of component signaature per entity, vector id = entity id
    std::vector<Signature> entityComponentSignatures;

    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

    std::set<Entity> entitiesToBeAdded;
    std::set<Entity> entitiesToBeKilled;

    std::deque<int> freeIds;

public:
    Registy() = default;
    Entity CreateEntity();
    void Update();
    template <typename TComponent,typename ...TArgs> void AddComponent(Entity entity,TArgs&& ...args);
    template<typename TComponent> void RemoveComponent(Entity entity);
    template<typename TComponent> bool HasComponent(Entity entity) const;
    template<typename TComponent> TComponent& GetComponent(Entity Entity) const;
    
    template<typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
    template<typename TSystem> void RemoveSystem();
    template<typename TSystem> bool HasSystem() const;
    template<typename TSystem> TSystem& GetSystem() const;

    void AddEntityToSystem(Entity entity);
    void RemoveEntityFromSystems(Entity entity);
    void KillEntity(Entity entity);
};

template <typename TComponent> void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId();
    ComponentSignature.set(componentId);
}
template <typename TComponent,typename ...TArgs> 
void Registy::AddComponent(Entity entity,TArgs&& ...args){
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    
    if (componentId >= static_cast<int>(componentPools.size())){
        componentPools.resize(componentId + 1 ,nullptr);
    }
    if (!componentPools[componentId]){
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }
    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    
    if (entityId >= componentPool->getSize()){
        componentPool->Resize(numEntities);
    }

    TComponent newComponent(std::forward<TArgs>(args)...);

    componentPool->Set(entityId,newComponent);
    entityComponentSignatures[entityId].set(componentId);
}
template <typename TComponent> 
void Registy::RemoveComponent(Entity entity){
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    entityComponentSignatures[entityId].set(componentId,false);
}
template <typename TComponent>
bool Registy::HasComponent(Entity entity) const {
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    return entityComponentSignatures[entityId].test(componentId);
}
template<typename TSystem, typename ...TArgs>
void Registy::AddSystem(TArgs&& ...args){
    //dont get new inside the ()? and why here type_index need typeid()?
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    newSystem->registry = this;
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template<typename TSystem> 
void Registy::RemoveSystem(){
    //find is come from what lib?
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template<typename TSystem> 
bool Registy::HasSystem() const{
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template<typename TSystem> 
TSystem& Registy::GetSystem() const{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    // dont get why *()
    return *(std::static_pointer_cast<TSystem>(system->second));
}
template<typename TComponent>
TComponent& Registy::GetComponent(Entity entity) const{
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();
    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    return componentPool->Get(entityId);
}
template <typename TComponent,typename ...TArgs> 
void Entity::AddComponent(TArgs&& ...args){
    registry->AddComponent<TComponent>(*this,std::forward<TArgs>(args)...);
}
template <typename TComponent> 
void Entity::RemoveComponent(){
    registry->RemoveComponent<TComponent>(*this);
}
template <typename TComponent>
bool Entity::HasComponent() const {
    return registry->HasComponent<TComponent>(*this);
}
template<typename TComponent>
TComponent& Entity::GetComponent() const{
    return registry->GetComponent<TComponent>(*this);
}

#endif
