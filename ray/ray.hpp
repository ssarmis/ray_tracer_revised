#pragma once

#include "math.hpp"

class Object;

struct Ray {
    Math::v3 origin;
    Math::v3 direction;
};

struct RayPayload {
    Object* closestHit;
    Math::v3 normal;
    Math::v3 position;
    r32 closestDistance;
};

