#include "object.hpp"
#include "scene.hpp"

using namespace Math;

Object::Object() : mPosition() {
    mMaterial = {};
    mIsMesh = false;
}

Object::Object(v3 position) : mPosition(position) {
    mMaterial = {};
    mIsMesh = false;
}

void Object::SetRotation(const m3& rotation){
    mRotation = rotation;
}

void Object::SetScale(const m3& scale){
    mScale = scale;
}

void Object::SetTranslate(const v3& translate) {
    mTranslate = translate;
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

bool Plane::IntersectsBox(Math::v3 position, Math::v3 size) {
    v3 max;
    max.x = position.x + size.x / 2.0f;
    max.y = position.y + size.y / 2.0f;
    max.z = position.z + size.z / 2.0f;

    v3 e = max - position;

    float r = e.x * std::abs(-mNormal.x) + e.y * std::abs(-mNormal.y) + e.z * std::abs(-mNormal.z);

    // Compute distance of box center from plane
    float s = v3::Dot(-mNormal, position) - mPosition.Length();

    // Intersection occurs when distance s falls within [-r,+r] interval
    return std::abs(s) <= r;
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

bool Sphere::IntersectsBox(Math::v3 position, Math::v3 size) {

    BoundingBox b(mPosition - mRadius, mPosition + mRadius);

    return Intersections::AABB(b, BoundingBox(position - size/3.0, position + size / 3.0));

    v3 min = position - (size / 1.5f);
    v3 max = position + (size / 1.5f);

    float r2 = mRadius * mRadius;
    r32 dmin = 0;

    if (mPosition.x < min.x) dmin += std::sqrt(mPosition.x - min.x);
    else if (mPosition.x > max.x) dmin += std::sqrt(mPosition.x - max.x);

    if (mPosition.y < min.y) dmin += std::sqrt(mPosition.y - min.y);
    else if (mPosition.y > max.y) dmin += std::sqrt(mPosition.y - max.y);

    if (mPosition.z < min.z) dmin += std::sqrt(mPosition.z - min.z);
    else if (mPosition.z > max.z) dmin += std::sqrt(mPosition.z - max.z);

    return dmin <= r2;
}

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

TriangleArray::TriangleArray(const std::vector<Triangle>& triangles) : mTriangles(triangles) {
    mIsMesh = true;
}

RayPayload TriangleArray::Intersect(const Ray& ray, const std::vector<Triangle*>& triangles) {
    RayPayload closestPayload;
    closestPayload.closestDistance = std::numeric_limits<float>::max();

    for (auto& tri: triangles) {
        v3 AB = tri->B - tri->A;
        v3 AC = tri->C - tri->A;

        v3 normal = v3::Cross(AB, AC);
        if (normal.z > 0) {
            v3 aux = tri->C;
            tri->C = tri->B;
            tri->B = aux;

            AB = tri->B - tri->A;
            AC = tri->C - tri->A;
            normal = v3::Cross(AB, AC);
        }

        r32 dirDot = v3::Dot(ray.direction, normal);
        if (dirDot < 0.000000001f && dirDot > 0.000000001f) {
            return Scene::Miss();
        }
        v3 originToOnePoint = tri->A - ray.origin;
        float t = v3::Dot(normal, originToOnePoint) / dirDot;
        if (t >= 0) {

            // Triangle check
            v3 point = (ray.origin + ray.direction * t);

            v3 v00 = tri->B - tri->A;
            v3 v01 = tri->C - tri->A;
            v3 v02 = point - tri->A;

            r32 d00 = v3::Dot(v00, v00);
            r32 d01 = v3::Dot(v00, v01);
            r32 d11 = v3::Dot(v01, v01);
            r32 d20 = v3::Dot(v02, v00);
            r32 d21 = v3::Dot(v02, v01);
            r32 denom = d00 * d11 - d01 * d01;
            r32 v = (d11 * d20 - d01 * d21) / denom;
            r32 w = (d00 * d21 - d01 * d20) / denom;
            r32 u = 1.0f - v - w;

            if (u >= 0 && v >= 0 && w >= 0) {
                if (t < closestPayload.closestDistance) {
                    closestPayload = Hit(ray, t, normal, point);
                }
            }
        }
    }
    if (closestPayload.closestDistance != std::numeric_limits<float>::max()) {
        return closestPayload;
    }
    return Scene::Miss();
}

RayPayload TriangleArray::Intersect(const Ray& ray) {
    RayPayload closestPayload;
    closestPayload.closestDistance = std::numeric_limits<float>::max();

    for (auto& triangle: mTriangles) {
        v3 AB = triangle.B - triangle.A;
        v3 AC = triangle.C - triangle.A;

        v3 normal = v3::Cross(AB, AC);
        if (normal.z > 0) {
            v3 aux = triangle.C;
            triangle.C = triangle.B;
            triangle.B = aux;

            AB = triangle.B - triangle.A;
            AC = triangle.C - triangle.A;
            normal = v3::Cross(AB, AC);
        }

        r32 dirDot = v3::Dot(ray.direction, normal);
        if (dirDot < 0.000000001f && dirDot > 0.000000001f) {
            return Scene::Miss();
        }
        v3 originToOnePoint = triangle.A - ray.origin;
        float t = v3::Dot(normal, originToOnePoint) / dirDot;
        if (t >= 0) {

            // Triangle check
            v3 point = (ray.origin + ray.direction * t);

            v3 v00 = triangle.B - triangle.A;
            v3 v01 = triangle.C - triangle.A;
            v3 v02 = point - triangle.A;

            r32 d00 = v3::Dot(v00, v00);
            r32 d01 = v3::Dot(v00, v01);
            r32 d11 = v3::Dot(v01, v01);
            r32 d20 = v3::Dot(v02, v00);
            r32 d21 = v3::Dot(v02, v01);
            r32 denom = d00 * d11 - d01 * d01;
            r32 v = (d11 * d20 - d01 * d21) / denom;
            r32 w = (d00 * d21 - d01 * d20) / denom;
            r32 u = 1.0f - v - w;

            if (u >= 0 && v >= 0 && w >= 0) {
                if (t < closestPayload.closestDistance) {
                    closestPayload = Hit(ray, t, normal, point);
                }
            }
            // Bad barycentric coordinate implementation below, kept for debugging later
#if 0
            v3 BC = (triangle.C - triangle.B);
            v3 CA = (triangle.A - triangle.C);
            v3 u = AB - v3::ProjectionAlongV3(AB, triangle.B - triangle.C);
            v3 v = BC - v3::ProjectionAlongV3(BC, triangle.C - triangle.A);
            v3 w = CA - v3::ProjectionAlongV3(CA, triangle.A - triangle.B);

            v3 AP = point - triangle.A;
            v3 BP = point - triangle.B;
            v3 CP = point - triangle.C;

            r32 xb = 1 - (v3::Project(AP, u) / v3::Project(AB, u));
            //r32 yb = 1 - (v3::Project(BP, v) / v3::Project(BC, v));
            r32 zb = 1 - (v3::Project(CP, w) / v3::Project(CA, w));
            r32 yb = 1 - xb - zb;
            //r32 zb = 1 - xb - yb;

            if (xb >= 0 && yb >= 0 && zb >= 0) {
                return Hit(ray, t, normal, point);
            }
#endif
#if 0
            //AB = AB.Normalized();



            v3 AP = point - triangle.A;
            v3 BP = point - triangle.B;
            v3 CP = point - triangle.C;
#if 0
            AB = -AB;
            BC = -BC;
            CA = -CA;
#endif
            // TODO make this show the ACTUAL triangle, now its a bad triangle
            if (v3::Dot(AP, AB) > 0 &&
                v3::Dot(BP, BC) > 0 &&
                v3::Dot(CP, CA) > 0) {
                return Hit(ray, t, normal, point);
            }
#endif
            //
        }
    }
    if (closestPayload.closestDistance != std::numeric_limits<float>::max()) {
        return closestPayload;
    }
    return Scene::Miss();
};

RayPayload TriangleArray::Hit(const Ray& ray, r32 t, v3 normal, v3 point) {
    RayPayload p;
    p.closestDistance = t;
    p.closestHit = this;
    p.normal = normal;
    p.position = point;
    return p;
}

void TriangleArray::ComputeBoundingBox(){
    r32 maxf = std::numeric_limits<float>::max();
    r32 minf = std::numeric_limits<float>::min();

    v3 min(maxf, maxf, maxf);
    v3 max(minf, minf, minf);

    for (const auto& triangle : mTriangles) {
        min.x = std::min(min.x, triangle.A.x);
        min.y = std::min(min.y, triangle.A.y);
        min.z = std::min(min.z, triangle.A.z);

        min.x = std::min(min.x, triangle.B.x);
        min.y = std::min(min.y, triangle.B.y);
        min.z = std::min(min.z, triangle.B.z);

        min.x = std::min(min.x, triangle.C.x);
        min.y = std::min(min.y, triangle.C.y);
        min.z = std::min(min.z, triangle.C.z);

        max.x = std::max(max.x, triangle.A.x);
        max.y = std::max(max.y, triangle.A.y);
        max.z = std::max(max.z, triangle.A.z);

        max.x = std::max(max.x, triangle.B.x);
        max.y = std::max(max.y, triangle.B.y);
        max.z = std::max(max.z, triangle.B.z);

        max.x = std::max(max.x, triangle.C.x);
        max.y = std::max(max.y, triangle.C.y);
        max.z = std::max(max.z, triangle.C.z);
    }
    mBoundingBox = BoundingBox(min, max);
    std::cout << mBoundingBox;
}

bool TriangleArray::IntersectsBox(Math::v3 position, Math::v3 size) {
    BoundingBox b(position - (size/ 2.0), position + (size / 2.0));

    if (Intersections::AABB(mBoundingBox, b)) {
        return true;
    }
    return false;
}

#include "tribox.hpp"
std::vector<Triangle*> TriangleArray::IntersectedTriangles(Math::v3 position, Math::v3 size) {
    std::vector<Triangle*> result;

    for (auto& t : mTriangles) {
        r32 verts[3][3] = {
            {t.A.x, t.A.y, t.A.z},
            {t.B.x, t.B.y, t.B.z},
            {t.C.x, t.C.y, t.C.z}
        };
        r32 c[3] = { position.x, position.y, position.z };
        r32 hs[3] = { size.x / 2, size.y / 2, size.z / 2 };

        bool intersected = triBoxOverlap(c, hs, verts) == 1;

        if (intersected) {
            result.push_back(&t);
        }
    }

    return result;
}

void TriangleArray::PushTransforms() {
    for (auto& triangle : mTriangles) {
        triangle.A = mRotation * triangle.A;
        triangle.B = mRotation * triangle.B;
        triangle.C = mRotation * triangle.C;

        triangle.A = mScale * triangle.A;
        triangle.B = mScale * triangle.B;
        triangle.C = mScale * triangle.C;

        triangle.A += mTranslate;
        triangle.B += mTranslate;
        triangle.C += mTranslate;
    }
}

Cube::Cube(Math::v3 position, Math::v3 size) : Object(position), mSize(size){
}

bool Cube::IntersectsBox(Math::v3 position, Math::v3 size) { return true; }

RayPayload Cube::Intersect(const Ray& r) {
    r32 tmin = std::numeric_limits<r32>::min();
    r32 tmax = std::numeric_limits<r32>::max();
    BoundingBox b(mPosition - (mSize / 2), mPosition - (mSize / 2));
    if (r.direction.x != 0.0) {
        r32 tx1 = (b.min.x - r.origin.x) / r.direction.x;
        r32 tx2 = (b.max.x - r.origin.x) / r.direction.x;

        tmin = std::max(tmin, std::min(tx1, tx2));
        tmax = std::min(tmax, std::max(tx1, tx2));
    }

    if (r.direction.y != 0.0) {
        r32 ty1 = (b.min.y - r.origin.y) / r.direction.y;
        r32 ty2 = (b.max.y - r.origin.y) / r.direction.y;

        tmin = std::max(tmin, std::min(ty1, ty2));
        tmax = std::min(tmax, std::max(ty1, ty2));
    }

    if (r.direction.z != 0.0) {
        r32 ty1 = (b.min.z - r.origin.z) / r.direction.z;
        r32 ty2 = (b.max.z - r.origin.z) / r.direction.z;

        tmin = std::max(tmin, std::min(ty1, ty2));
        tmax = std::min(tmax, std::max(ty1, ty2));
    }

    if (tmax >= tmin) {
        return Hit(r, tmin);
    }

    return Scene::Miss();
}

RayPayload Cube::Hit(const Ray& ray, r32 t) {
    RayPayload result = {};

    v3 intersectionPoint = ray.origin + ray.direction * t;
    v3 normal = intersectionPoint - mPosition;
#if 0
    normal.x = (normal.x > normal.y) ? ((normal.x > normal.z) ? normal.x : 0) : 0;
    normal.y = (normal.y > normal.x) ? ((normal.y > normal.z) ? normal.y : 0) : 0;
    normal.z = (normal.z > normal.y) ? ((normal.z > normal.x) ? normal.z : 0) : 0;
#endif
    normal = normal.Normalized();

    result.closestHit = this;
    result.closestDistance = t;
    result.position = intersectionPoint;
    result.normal = normal;

    return result;
}

