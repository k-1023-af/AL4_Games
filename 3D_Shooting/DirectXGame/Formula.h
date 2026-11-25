#pragma once
#include <numbers>
#include <cmath>
#include <algorithm>
#include "KamataEngine.h"

using namespace KamataEngine;

class Formula {};

const float halfPi = std::numbers::pi_v<float> / 2.0f;

struct AABB {
	Vector3 min;
	Vector3 max;

	void Fix() {
		if (min.x > max.x) std::swap(min.x, max.x);
		if (min.y > max.y) std::swap(min.y, max.y);
		if (min.z > max.z) std::swap(min.z, max.z);
	}
};

struct Line {
	Vector3 origin;
	Vector3 diff;
};

float Length(const Vector3& v1, const Vector3& v2);
float Lerp(float start, float end, float t);
float EaseOut(float start, float end, float t);
float EaseIn(float start, float end, float t);


Vector3 Add(const Vector3& v1, const Vector3& v2);
Vector3 Add(const Vector3& v, float f);
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
Vector3 Multiply(const Vector3& v, float scalar);
Vector3 Multiply(const Vector3& v1, const Vector3& v2);
float Dot(const Vector3& v1, const Vector3& v2);
Vector3 Normalize(const Vector3& v);
Vector3 Cross(const Vector3& v1, const Vector3& v2);
Vector3 Project(const Vector3& v1, const Vector3& v2);
Vector3 Clamp(const Vector3& value, const Vector3& min, const Vector3& max);
Vector3 Lerp(const Vector3& start, const Vector3& end, float t);
Vector3 EaseOut(const Vector3& start, const Vector3& end, float t);
Vector3 EaseIn(const Vector3& start, const Vector3& end, float t);
Vector3 Transform(const Vector3& v, const Matrix4x4& m);

Vector4 ToColor(uint32_t color);

Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b);
Matrix4x4 MakeScale(const Vector3& scale);
Matrix4x4 MakeRotatedXYZ(const Vector3& rotation);
Matrix4x4 MakeRotatedXYZ(const Matrix4x4& x, const Matrix4x4& y, const Matrix4x4& z);
Matrix4x4 MakeRotatedX(const float radian);
Matrix4x4 MakeRotatedY(const float radian);
Matrix4x4 MakeRotatedZ(const float radian);
Matrix4x4 MakeTranslation(const Vector3& translation);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotation, const Vector3& translation);


bool IsCollisionAABB(const AABB& a, const AABB& b);
bool IsCollision(const Line& line, const AABB& aabb);
//AABB GetAABB();

//inline float RandomFloat(float min, float max) {
//	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
//}


////Binary Operator
//Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Add(v1, v2); }
//Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subtract(v1, v2); }
//Vector3 operator*(const Vector3& v, float s) { return Multiply(v, s); }
//Vector3 operator/(const Vector3& v, float s) { return Multiply(v, 1.0f / s); }
//Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) { return Add(m1, m2); }
//Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) { return Subtract(m1, m2); }
//Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) { return Multiply(m1, m2); }
//
////Unary Operator
//Vector3 operator-(const Vector3& v) { return { -v.x, -v.y, -v.z }; }
//Vector3 operator+(const Vector3& v) { return v; }