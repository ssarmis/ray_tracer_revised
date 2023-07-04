#include "object.hpp"
#include "scene.hpp"

using namespace Math;

Object::Object(v3 position) : mPosition(position) {
    mMaterial = {};
}

void Object::SetColor(v3 color) {
    mMaterial.albedo = color;
}
void Object::SetRoughness(r32 rough) {
    mMaterial.roughness = rough;
}
void Object::SetAlbedoTexture(Texture* texture) {
    mMaterial.hasAlbedoTexture = true;
    mMaterial.albedoTexture = texture;
}

void Object::SetEmission(v3 emission) {
    mMaterial.hasEmission = true;
    mMaterial.emission = emission;
}

RayPayload Plane::Intersect(const Ray& ray) {
    r32 dirDot = v3::Dot(ray.direction, mNormal);
    if (dirDot < 0.000000001f && dirDot > 0.000000001f) {
        return Scene::Miss();
    }
    v3 originToPlane = mPosition - ray.origin;
    float t = v3::Dot(mNormal, originToPlane) / dirDot;
    if (t >= 0){
        return Hit(ray, t);
    }
    return Scene::Miss();
}

RayPayload Plane::Hit(const Ray& ray, r32 t) {
    RayPayload result = {};

    v3 intersectionPoint = ray.origin + ray.direction * t;
    v3 normal = mNormal;
    normal = normal.Normalized();

    result.closestHit = this;
    result.closestDistance = t;
    result.position = intersectionPoint;
    result.normal = normal;

    return result;
}

Sphere::Sphere(v3 position, r32 radius) : Object(position), mRadius(radius) {}

RayPayload Sphere::Intersect(const Ray& ray) {
    v3 o = ray.origin - mPosition;

    r32 a = v3::Dot(ray.direction, ray.direction);
    r32 b = 2 * v3::Dot(o, ray.direction);
    r32 c = v3::Dot(o, o) - mRadius * mRadius;

    r32 delta = b * b - 4 * a * c;
    r32 cd = std::sqrt(delta);
    if (delta >= 0) {
        r32 t = (-b - cd) / (2 * a);
        if (t <= 0) {
            return Scene::Miss();
        }

        return Hit(ray, t);
    }

    return Scene::Miss();
}

RayPayload Sphere::Hit(const Ray& ray, r32 t) {
    RayPayload result = {};

    v3 intersectionPoint = ray.origin + ray.direction * t;
    v3 normal = intersectionPoint - mPosition;
    normal = normal.Normalized();

    result.closestHit = this;
    result.closestDistance = t;
    result.position = intersectionPoint;
    result.normal = normal;

    return result;
}
