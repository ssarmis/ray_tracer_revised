#include "math.hpp"
#include "ray.hpp"

bool Intersections::RayAABB(Math::BoundingBox b, Ray r){
	r32 tmin = std::numeric_limits<r32>::min();
	r32 tmax = std::numeric_limits<r32>::max();

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

	return tmax >= tmin;
}