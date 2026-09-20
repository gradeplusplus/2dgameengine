#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H
#include "Event.h"
#include "../ECS/ECS.h"
struct CollisionEvent : public Event {
    Entity a, b;
    CollisionEvent(Entity a, Entity b) : a(a), b(b) {}
};
#endif
