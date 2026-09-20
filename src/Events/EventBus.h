#ifndef EVENTBUS_H
#define EVENTBUS_H
#include "Event.h"
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <functional>

class EventBus {
public:
    template <typename TEvent>
    static void Subscribe(std::function<void(TEvent&)> handler) {
        auto type = std::type_index(typeid(TEvent));
        subscribers[type].push_back([handler](Event& e){
            handler(static_cast<TEvent&>(e));
        });
    }
    template <typename TEvent, typename... TArgs>
    static void Emit(TArgs&&... args) {
        TEvent event(std::forward<TArgs>(args)...);
        auto it = subscribers.find(std::type_index(typeid(TEvent)));
        if (it != subscribers.end())
            for (auto& handler : it->second) handler(event);
    }
private:
    inline static std::unordered_map<std::type_index,
        std::vector<std::function<void(Event&)>>> subscribers;
};
#endif
