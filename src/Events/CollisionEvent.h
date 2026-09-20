#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H
#include "Event.h"
#include "../ECS/ECS.h"
#include <string>
struct CollisionEvent : public Event {
    Entity a, b;
    std::string aTag, bTag;
    bool isTrigger;
    CollisionEvent(Entity a, Entity b, const std::string& aTag,
                   const std::string& bTag, bool isTrigger)
        : a(a), b(b), aTag(aTag), bTag(bTag), isTrigger(isTrigger) {}
};
#endif
