#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>


class Function {};

const float halfPi = std::numbers::pi_v<float> / 2.0f;

struct Vector3 {
	float x;
	float y;
	float z;

	// constructor
	Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

	// Put Compound Assignment Operators inside the class
	Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
	Vector3& operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vector3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
	Vector3& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }
};

struct Vector4 {
	float x;
	float y;
	float z;
	float w;
	//constructor
	Vector4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f) : x(x), y(y), z(z), w(w){}
};

struct Matrix4x4 {
	float m[4][4];
};

struct AABB {
	Vector3 min;
	Vector3 max;

	void Fix() {
		if (min.x > max.x) std::swap(min.x, max.x);
		if (min.y > max.y) std::swap(min.y, max.y);
		if (min.z > max.z) std::swap(min.z, max.z);
	}
};


float Length(const Vector3& v1, const Vector3& v2);

Vector3 Add(const Vector3& v1, const Vector3& v2);
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
Vector3 MultiplyByS(const Vector3& v, float scalar);
Vector3 Multiply2Vectors(const Vector3& v1, const Vector3& v2);
float Dot(const Vector3& v1, const Vector3& v2);
Vector3 Normalize(const Vector3& v);
Vector3 Cross(const Vector3& v1, const Vector3& v2);
Vector3 Project(const Vector3& v1, const Vector3& v2);
Vector3 Clamp(const Vector3& value, const Vector3& min, const Vector3& max);
Vector3 Lerp(const Vector3& start, const Vector3& end, float t);


Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b);


bool isCollisionAABB(const AABB& a, const AABB& b);









//Binary Operator
Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subtract(v1, v2); }
Vector3 operator*(const Vector3& v, float s) { return MultiplyByS(v, s); }
Vector3 operator*(const Vector3& v, float s) { return v * s; }
Vector3 operator/(const Vector3& v, float s) { return MultiplyByS(v, 1.0f / s); }
Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) { return Add(m1, m2); }
Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) { return Subtract(m1, m2); }
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) { return Multiply(m1, m2); }

//Unary Operator
Vector3 operator-(const Vector3& v) { return { -v.x, -v.y, -v.z }; }
Vector3 operator+(const Vector3& v) { return v; }