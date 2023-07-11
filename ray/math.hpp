#pragma once

#include "global.hpp"

#include <cmath>
#include <iostream>

class Ray;

namespace Random {
	extern u32 state;

	static u32 Rand(u32& state) {
		state ^= (state << 13);
		state ^= (state >> 17);
		state ^= (state << 5);
		return state;
	}

	static u32 Rand() {
		return Rand(state);
	}
}

namespace Math {
	static constexpr r32 PI = 3.14159265358979323846264338327950288;

	struct v2 {
		r32 x;
		r32 y;

		v2() :x(0), y(0) {}
		v2(r32 x, r32 y) :x(x), y(y) {}

		friend v2 operator-(const v2& l, const v2& r) {
			return v2(l.x - r.x, l.y - r.y);
		}
		friend v2 operator+(const v2& l, const v2& r) {
			return v2(l.x + r.x, l.y + r.y);
		}
	
		static r32 Dot(const v2& l, const v2& r) {
			return l.x * r.x + l.y * r.y;
		}

		r32 Length() const { return std::sqrt(x * x + y * y); }

		v2 Normalized() {
			r32 length = std::sqrt(x * x + y * y);
			return v2(x / length, y / length);
		}
	};

	struct v3 {
		enum class Axies {
			X,
			Y,
			Z
		};
		r32 x;
		r32 y;
		r32 z;

		v3() :x(0), y(0), z(0) {}
		v3(r32 x, r32 y, r32 z) :x(x), y(y), z(z) {}
		v3(const v3& o) :x(o.x), y(o.y), z(o.z) {}

		friend v3 operator-(v3& v) {
			return v3(-v.x, -v.y, -v.z);
		}
		friend v3 operator-(const v3& l, const v3& r) {
			return v3(l.x - r.x, l.y - r.y, l.z - r.z);
		}
		friend v3 operator-(const v3& l, const r32& r) {
			return v3(l.x - r, l.y - r, l.z - r);
		}
		friend v3 operator+(const v3& l, const v3& r) {
			return v3(l.x + r.x, l.y + r.y, l.z + r.z);
		}
		friend v3 operator+(const v3& l, const r32& r) {
			return v3(l.x + r, l.y + r, l.z + r);
		}

		void operator+=(const v3& r) {
			x += r.x;
			y += r.y;
			z += r.z;
		}
		void operator*=(const v3& r) {
			x *= r.x;
			y *= r.y;
			z *= r.z;
		}

		static v3 Rand() {
			r32 xx = (r32)(std::rand() % 10) / 10.0;
			r32 yy = (r32)(std::rand() % 10) / 10.0;
			r32 zz = (r32)(std::rand() % 10) / 10.0;
			xx = xx * 2 - 1;
			yy = yy * 2 - 1;
			zz = zz * 2 - 1;
			return v3(xx, yy, zz);
		}

		static v3 RandUnitCircle() {
			r32 xx = (r32)Random::Rand() / (r32)std::numeric_limits<u32>::max();
			r32 yy = (r32)Random::Rand() / (r32)std::numeric_limits<u32>::max();
			r32 zz = (r32)Random::Rand() / (r32)std::numeric_limits<u32>::max();

			xx = xx * 2 - 1;
			yy = yy * 2 - 1;
			zz = zz * 2 - 1;

			return v3(xx, yy, zz);
		}

		static v3 RandUnitCircle(r32 l, r32 r) {
			r32 xx = (r32)Random::Rand() / (r32)std::numeric_limits<u32>::max();
			r32 yy = (r32)Random::Rand() / (r32)std::numeric_limits<u32>::max();
			r32 zz = (r32)Random::Rand() / (r32)std::numeric_limits<u32>::max();

			xx = xx * 2 - 1;
			yy = yy * 2 - 1;
			zz = zz * 2 - 1;

			v3 result(xx, yy, zz);
			result.Clamp(l, r);
			return result;
		}

		static v3 Reflect(v3 d, v3 n) {
			return d + n * (-2) * v3::Dot(d, n);
		}
#if 0
		static v3 Reflect(v3 d, v3 n) {
			return d + n * 2;
		}
#endif
		static v3 Cross(const v3& l, const v3& r) {
			v3 result;
			result.x = l.y * r.z - l.z * r.y;
			result.y = l.z * r.x - l.x * r.z;
			result.z = l.x * r.y - l.y * r.x;
			return result;
		}

		void Clamp(r32 l, r32 r) {
			x = std::min(r, std::max(l, x));
			y = std::min(r, std::max(l, y));
			z = std::min(r, std::max(l, z));
		}

		static r32 Dot(const v3& l, const v3& r) {
			return l.x * r.x + l.y * r.y + l.z * r.z;
		}

		friend v3 operator*(const v3& l, const r32& r) {
			return v3(l.x * r, l.y * r, l.z * r);
		}
		friend v3 operator*(const r32& l, const v3& r) {
			return v3(r.x * l, r.y * l, r.z * l);
		}

		v3& operator=(const v3& o) {
			x = o.x;
			y = o.y;
			z = o.z;
			return *this;
		}

		static r32 Project(const v3& a, const v3& b) {
			return Dot(a, b) / b.Length();
		}

		static v3 ProjectionAlongV3(const v3& a, const v3& target) {
			float projectionSize = Project(a, target);
			return target * projectionSize;
		}

		friend std::ostream& operator<< (std::ostream & stream, const v3& v) {
			stream << "[" << v.x << ", " << v.y << ", " << v.z << "]" << std::endl;
			return stream;
		}

		friend v3 operator/(const v3& l, const r32& r) {
			return v3(l.x / r, l.y / r, l.z / r);
		}

		r32 Length() const { return std::sqrt(x * x + y * y + z * z); }

		v3 Normalized() {
			r32 length = std::sqrt(x * x + y * y + z * z);
			return v3(x / length, y / length, z / length);
		}

		static v3 Hadamard(const v3& l, const v3& r) {
			return v3(l.x * r.x, l.y * r.y, l.z * r.z);
		}
	};

	struct Triangle {
		v3 A;
		v3 B;
		v3 C;
		Triangle(v3 a, v3 b, v3 c) :A(a), B(b), C(c) {}
		Triangle(const Triangle& o) :A(o.A), B(o.B), C(o.C) {}
	};

	struct m3 {
		r32 m[3 * 3];
		m3() :m3(1.0f) {}
		m3(r32 scale) : m{} {
			m[0 + 0 * 3] = scale;
			m[1 + 1 * 3] = scale;
			m[2 + 2 * 3] = scale;
		}
#if 0

		friend v3 operator*(const v3& vec, const m3& mat) {
			v3 result;

			result.x = vec.x * mat.m[0] +
				vec.y * mat.m[0 + 1 * 3] +
				vec.z * mat.m[0 + 2 * 3];

			result.y = vec.x * mat.m[1] +
				vec.y * mat.m[1 + 1 * 3] +
				vec.z * mat.m[1 + 2 * 3];

			result.z = vec.x * mat.m[2] +
				vec.y * mat.m[2 + 1 * 3] +
				vec.z * mat.m[2 + 2 * 3];

			return result;
		}
#endif

		friend v3 operator*(const m3& mat, const v3& vec) {
			v3 result;

			result.x = vec.x * mat.m[0 + 0 * 3] +
				vec.y * mat.m[1 + 0 * 3] +
				vec.z * mat.m[2 + 0 * 3];

			result.y = vec.x * mat.m[0 + 1 * 3] +
				vec.y * mat.m[1 + 1 * 3] +
				vec.z * mat.m[2 + 1 * 3];

			result.z = vec.x * mat.m[0 + 2 * 3] +
				vec.y * mat.m[1 + 2 * 3] +
				vec.z * mat.m[2 + 2 * 3];

			return result;
		}

		friend m3 operator*(const m3& a, const m3& b) {
			m3 result;

			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					for (int k = 0; k < 3; ++k) {
						result.m[i + j * 3] += a.m[k + i * 3] * b.m[j + k * 3];
					}
				}
			}

			return result;
		}

		static m3 Rotate(const r32& degrees, const v3::Axies& axies) {
			r32 d2r = degrees * (PI / 180.0f);
			m3 result;
			switch (axies) {
				case v3::Axies::X: {
					result.m[0] = 1;
					
					result.m[1 + 1 * 3] = std::cos(d2r);
					result.m[2 + 1 * 3] = -std::sin(d2r);
					
					result.m[1 + 2 * 3] = std::sin(d2r);
					result.m[2 + 2 * 3] = std::cos(d2r);
					break;
				}

				case v3::Axies::Y: {
					result.m[0 + 0 * 3] = std::cos(d2r);
					result.m[2 + 0 * 3] = std::sin(d2r);

					result.m[1 + 1 * 3] = 1;

					result.m[0 + 2 * 3] = -std::sin(d2r);
					result.m[2 + 2 * 3] = std::cos(d2r);
					break;
				}

				case v3::Axies::Z: {
					result.m[0 + 0 * 3] = std::cos(d2r);
					result.m[1 + 0 * 3] = -std::sin(d2r);

					result.m[0 + 1 * 3] = std::sin(d2r);
					result.m[1 + 1 * 3] = std::cos(d2r);

					result.m[2 + 2 * 3] = 1;
					break;
				}
			}
			return result;
		}

		static m3 Scale(r32 x, r32 y, r32 z) {
			m3 result;
			result.m[0 + 0 * 3] = x;
			result.m[1 + 1 * 3] = y;
			result.m[2 + 2 * 3] = z;
			return result;
		}

		static m3 Identity() {
			return m3();
		}
	};

	struct BoundingBox {
		v3 min;
		v3 max;
		v3 position;
		v3 size;
		//BoundingBox(v3 p, v3 s) : min(position - (size / 2)), max(position + (size / 2)), position(p), size(p) {}
		BoundingBox() {}
		BoundingBox(v3 mi, v3 ma) {
			min = mi;
			max = ma;
			position = (min + max) / 2;
			size = max - min;
		}
		BoundingBox(const BoundingBox& o) :min(o.min), max(o.max), position(o.position), size(o.size) {}

		friend std::ostream& operator<<(std::ostream& stream, const BoundingBox& box) {
			std::cout << "[\nmin: " << box.min << "max: " << box.max << "]" << std::endl;
			return stream;
		}
	};
}

namespace RGB {
	inline static Math::v3 RGB2SRGB(const Math::v3& color) {
		Math::v3 result;
		result.x = std::pow(color.x, 1.0 / 2.2);
		result.y = std::pow(color.y, 1.0 / 2.2);
		result.z = std::pow(color.z, 1.0 / 2.2);
		return result;
	}
	inline static Math::v3 BufferToR32V3(u8* data) {
		r32 r = data[0] / 255.0f;
		r32 g = data[1] / 255.0f;
		r32 b = data[2] / 255.0f;

		return Math::v3(r, g, b);
	}
}

namespace Intersections {
	static bool AABB(const Math::BoundingBox& a, const Math::BoundingBox& b) {
		return (
			a.min.x <= b.max.x &&
			a.max.x >= b.min.x &&
			a.min.y <= b.max.y &&
			a.max.y >= b.min.y &&
			a.min.z <= b.max.z &&
			a.max.z >= b.min.z
			);
	}

	bool RayAABB(Math::BoundingBox b, Ray r);
}