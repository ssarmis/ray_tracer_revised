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

    Math::m3 mRotation;
    Math::v3 mTranslate;
    Math::m3 mScale;

    bool mIsMesh;

    Object();
    Object(Math::v3 position);

    void SetRotation(const Math::m3& rotation);
    void SetScale(const Math::m3& scale);
    void SetTranslate(const Math::v3& translate);
    void SetColor(Math::v3 color);
    void SetRoughness(r32 rough);
    void SetEmission(Math::v3 emission);
    void SetAlbedoTexture(Texture* texture);

    virtual bool IntersectsBox(Math::v3 position, Math::v3 size) = 0;
    virtual RayPayload Intersect(const Ray& ray) = 0;
    virtual RayPayload Hit(const Ray& ray, r32 t) = 0;
};

class MeshObject : public Object {
public:
    MeshObject() {
        mIsMesh = true;
    }
    // shouldn't be virtual, but for the sake of cleanness, I will implement this
    // in the triangle array class
    // TODO make the mesh object the only class, no Triangle Array class
    virtual std::vector<Math::Triangle*> IntersectedTriangles(Math::v3 position, Math::v3 size) = 0;
};

class Plane : public Object {
public:
    Math::v3 mNormal;
    Plane(Math::v3 position, Math::v3 normal) : Object(position), mNormal(normal) {}

    bool IntersectsBox(Math::v3 position, Math::v3 size) override;
    RayPayload Intersect(const Ray& ray) override;
    RayPayload Hit(const Ray& ray, r32 t) override;
};

class Sphere : public Object {
public:
    r32 mRadius;
    Sphere(Math::v3 position, r32 radius);

    bool IntersectsBox(Math::v3 position, Math::v3 size);

    RayPayload Intersect(const Ray& ray) override;
    RayPayload Hit(const Ray& ray, r32 t) override;
};

class Cube : public Object {
public:
    Math::v3 mSize;
    Cube(Math::v3 position, Math::v3 size);

    bool IntersectsBox(Math::v3 position, Math::v3 size);

    RayPayload Intersect(const Ray& ray) override;
    RayPayload Hit(const Ray& ray, r32 t) override;
};

class TriangleArray : public MeshObject {
public:
    // BVH* bvh;
    Math::BoundingBox mBoundingBox;
    std::vector<Math::Triangle> mTriangles;
    TriangleArray(const std::vector<Math::Triangle>& triangles);

    void ComputeBoundingBox();

    std::vector<Math::Triangle*> IntersectedTriangles(Math::v3 position, Math::v3 size) override;
    bool IntersectsBox(Math::v3 position, Math::v3 size);

    void PushTransforms();
    RayPayload Intersect(const Ray& ray, const std::vector<Math::Triangle*>& triangles);
    RayPayload Intersect(const Ray& ray) override;
    RayPayload Hit(const Ray& ray, r32 t, Math::v3 normal, Math::v3 point);
    RayPayload Hit(const Ray& ray, r32 t) override { return RayPayload(); }
};
