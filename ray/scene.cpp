#include "scene.hpp"
#include "ray.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include "texture.hpp"

using namespace Math;

void Scene::AddObject(Object* o) {
    mObject.push_back(o);
}

RayPayload Scene::Miss() {
    RayPayload result;
    result.closestDistance = -1;
    return result;
}

// maybe in some renderer class
v3 Scene::ProcessPixel(int x, int y, int width, int height, v3 origin, v3 bias) {
    r32 aspectRatio = width / (r32)height;

    r32 u = ((x + 0.5f) / (r32)(width)) * aspectRatio;
    r32 v = (height - 1 - (y + 0.5f)) / (r32)(height);
    u = u * 2 - 1;
    v = v * 2 - 1;

    v3 Y(0, 1, 0);
    v3 X(1, 0, 0);
    v3 Z(0, 0, 1);

    r32 filmW = 5;
    r32 filmH = 5;
    r32 hfw = filmW / 2.0;
    r32 hfh = filmH / 2.0;

    r32 distance = 3.0f;
    v3 p = X * u * hfw + Y * v * hfh + Z * distance;
    //origin.x = p.x;
    //origin.y = p.y;
    v3 direction = p - origin + bias;
    direction = direction.Normalized();

    Ray r;
    r.direction = direction;
    r.origin = origin;

    v3 color;
    v3 skyColor(1, 0, 1);

    r32 factor = 1;
    v3 attenuation(1, 1, 1);

    int bounces = 4;
    r32 iterations = 1;
    for (int i = 0; i < bounces; ++i) {
        RayPayload p = CastRay(r);

        //++iterations;
        if (p.closestDistance < 0) {
            color += v3::Hadamard(attenuation, skyColor);
            break;
        }

        v3 c = p.closestHit->mMaterial.albedo;
        if (p.closestHit->mMaterial.hasAlbedoTexture) {
            // sphere
            v3 d = -p.normal;

            r32 chu = 0.5f + (std::atan2(d.z, d.x) / (2 * M_PI));
            r32 chv = 0.5f + (std::asin(d.y) / M_PI);

            c = p.closestHit->mMaterial.albedoTexture->GetColor(chu, chv);
            //
        }

        v3 em = p.closestHit->mMaterial.emission;
        color += v3::Hadamard(attenuation, em);
        
        attenuation = v3::Hadamard(attenuation, c);


        //color += c;

        if (p.closestHit->mMaterial.hasEmission) { // don't bounce from emitters
            break;
        }

        v3 nextDirection = v3::Reflect(r.direction, p.normal + v3::RandUnitCircle() * p.closestHit->mMaterial.roughness);
        r.direction = nextDirection;
        r.origin = p.position + p.normal * 0.000001f;
    }
    return color / iterations;
}

RayPayload Scene::CastRay(Ray& ray) {
    RayPayload closestHit;
    closestHit.closestDistance = std::numeric_limits<float>::max();

    bool hit = false;

    for (auto& obj : mObject) {
        RayPayload p = obj->Intersect(ray);
        if (p.closestDistance < closestHit.closestDistance && p.closestDistance > 0) {
            closestHit = p;
            hit = true;
        }
    }
    if (!hit) {
        closestHit.closestDistance = -1;
    }
    return closestHit;
}
