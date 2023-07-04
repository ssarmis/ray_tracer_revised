#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

#include "global.hpp"
#include "math.hpp"
#include "object.hpp"

class Scene {
public:
    std::vector<Object*> mObject;
    Math::v3* mPaths;
    int mIterations;

    void AddObject(Object* o);
    
    static RayPayload Miss();
    
    Math::v3 ProcessPixel(int x, int y, int width, int height, Math::v3 origin, Math::v3 bias);
    RayPayload CastRay(Ray& ray);
};
