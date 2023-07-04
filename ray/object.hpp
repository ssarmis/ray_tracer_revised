#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

#include "global.hpp"
#include "math.hpp"
#include "material.hpp"
#include "ray.hpp"

class Object {
public:
    Math::v3 mPosition;
    Material mMaterial;
    Object(Math::v3 position);

    void SetColor(Math::v3 color);
    void SetRoughness(r32 rough);
    void SetEmission(Math::v3 emission);
    void SetAlbedoTexture(Texture* texture);

    virtual RayPayload Intersect(const Ray& ray) = 0;
    virtual RayPayload Hit(const Ray& ray, r32 t) = 0;
};
class Plane : public Object {
public:
    Math::v3 mNormal;
    Plane(Math::v3 position, Math::v3 normal) : Object(position), mNormal(normal) {}

    RayPayload Intersect(const Ray& ray) override;
    RayPayload Hit(const Ray& ray, r32 t) override;
};

class Sphere : public Object {
public:
    r32 mRadius;
    Sphere(Math::v3 position, r32 radius);
    RayPayload Intersect(const Ray& ray) override;
    RayPayload Hit(const Ray& ray, r32 t) override;
};
