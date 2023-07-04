#pragma once

#include "global.hpp"

#include <cmath>
#include <iostream>

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
		friend v3 operator+(const v3& l, const v3& r) {
			return v3(l.x + r.x, l.y + r.y, l.z + r.z);
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

		//v3 operator-() {
		//	return v3(-x, -y, -z);
		//}

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