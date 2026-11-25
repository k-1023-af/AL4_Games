#include "Formula.h"




float Length(const Vector3& v1, const Vector3& v2) {
	float length = sqrtf(
		(v1.x - v2.x) * (v1.x - v2.x) +
		(v1.y - v2.y) * (v1.y - v2.y) +
		(v1.z - v2.z) * (v1.z - v2.z));
	return length;
}

float Lerp(float start, float end, float t) {
	//t = std::clamp(t, 0.0f, 1.0f);
	return start + (end - start) * t;
}

float EaseOut(float start, float end, float t) {
	t = std::clamp(t, 0.0f, 1.0f);
	//return start + (end - start) * (1 - std::pow(1 - t, 3));
	return start + (1.0f - (1.0f - t) * (1.0f - t)) * (end - start);
}

float EaseIn(float start, float end, float t) {
	t = std::clamp(t, 0.0f, 1.0f);
	//return start + (end - start) * std::pow(t, 3);
	if (t < 0.5f) {
		return start + (2.0f * t * t) * (end - start);
	}
	else {
		return start + (1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) * 0.5f) * (end - start);
	}
}


Vector3 Add(const Vector3& v1, const Vector3& v2) {
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

Vector3 Add(const Vector3& v, float f) {
	return { v.x + f, v.y + f, v.z + f };
}

Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

Vector3 Multiply(const Vector3& v, float scalar) {
	return { v.x * scalar, v.y * scalar, v.z * scalar };
}
Vector3 Multiply(float scalar, const Vector3& v) {
	return { v.x * scalar, v.y * scalar, v.z * scalar };
}

Vector3 Multiply(const Vector3& v1, const Vector3& v2) {
	return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
}

float Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 Normalize(const Vector3& v) {
	float length = std::sqrt(Dot(v, v));  // or std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z)

	if (length == 0.0f) {
		return { 0.0f, 0.0f, 0.0f };
	}
	return Multiply(v, 1.0f / length);
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

Vector3 Project(const Vector3& v1, const Vector3& v2) {
	float dot = Dot(v1, v2); //dot alignment 
	float lenSq = Dot(v2, v2); //length

	if (lenSq != 0.0f) { //do not divide by zero
		float t = dot / lenSq;

		t = std::clamp(t, 0.0f, 1.0f);

		Vector3 p = Multiply(v2, t);
		return p;
	}
	else return { 0.0f, 0.0f, 0.0f };
}

Vector3 Clamp(const Vector3& value, const Vector3& min, const Vector3& max) {
	Vector3 result;
	result.x = std::clamp(value.x, min.x, max.x);
	result.y = std::clamp(value.y, min.y, max.y);
	result.z = std::clamp(value.z, min.z, max.z);
	return result;
}

Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
	Vector3 result;
	t = std::clamp(t, 0.0f, 1.0f);

	result.x = start.x + (end.x - start.x) * t;
	result.y = start.y + (end.y - start.y) * t;
	result.z = start.z + (end.z - start.z) * t;

	return result;
}

Vector3 EaseOut(const Vector3& start, const Vector3& end, float t) {
	t = std::clamp(t, 0.0f, 1.0f);
	//return start + (end - start) * (1 - std::pow(1 - t, 3));
	Vector3 result = {
		start.x + (1.0f - (1.0f - t) * (1.0f - t)) * (end.x - start.x),
		start.y + (1.0f - (1.0f - t) * (1.0f - t)) * (end.y - start.y),
		start.z + (1.0f - (1.0f - t) * (1.0f - t)) * (end.z - start.z),
	};
	return result;
}

Vector3 EaseIn(const Vector3& start, const Vector3& end, float t) {
	t = std::clamp(t, 0.0f, 1.0f);
	//return start + (end - start) * std::pow(t, 3);
	Vector3 result;
	if (t < 0.5f) {
		result = {
			start.x + (2.0f * t * t) * (end.x - start.x), 
			start.y + (2.0f * t * t) * (end.y - start.y),
			start.z + (2.0f * t * t) * (end.z - start.z),
		};
	}
	else {
		result = { 
			start.x + (1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) * 0.5f) * (end.x - start.x),
			start.y + (1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) * 0.5f) * (end.y - start.y),
			start.z + (1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) * 0.5f) * (end.z - start.z),
		};
	}
	return result;
}

Vector3 Transform(const Vector3& v, const Matrix4x4& m) {
	Vector3 result;
	result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0];
	result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1];
	result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2];
	return result;
}

Vector4 ToColor(uint32_t color)
{
	float a = ((color >> 24) & 0xFF) / 255.0f;
	float r = ((color >> 16) & 0xFF) / 255.0f;
	float g = ((color >> 8) & 0xFF) / 255.0f;
	float b = ((color >> 0) & 0xFF) / 255.0f;
	return { r, g, b, a }; // KamataEngine::Vector4 is (x,y,z,w) = (r,g,b,a)
}



Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return result;
}

Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
	return result;
}

Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b) {
	Matrix4x4 result{};
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			result.m[row][column] = a.m[row][0] * b.m[0][column] + a.m[row][1] * b.m[1][column] + a.m[row][2] * b.m[2][column] + a.m[row][3] * b.m[3][column];
		}
	}
	return result;
}

Matrix4x4 MakeScale(const Vector3& scale) {
	Matrix4x4 result = { 0 };
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 MakeRotatedXYZ(const Matrix4x4& x, const Matrix4x4& y, const Matrix4x4& z) {
	Matrix4x4 result = Multiply(x, Multiply(y, z));
	return result;
}

Matrix4x4 MakeRotatedXYZ(const Vector3& rotate) {
	Matrix4x4 rotatedX = MakeRotatedX(rotate.x);
	Matrix4x4 rotatedY = MakeRotatedY(rotate.y);
	Matrix4x4 rotatedZ = MakeRotatedZ(rotate.z);

	Matrix4x4 result = Multiply(rotatedX, Multiply(rotatedY, rotatedZ));
	return result;
}

Matrix4x4 MakeRotatedX(const float radian) {
	Matrix4x4 result;
	result.m[0][0] = 1;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}

Matrix4x4 MakeRotatedY(const float radian) {
	Matrix4x4 result;
	result.m[0][0] = std::cos(radian);
	result.m[0][1] = 0;
	result.m[0][2] = -std::sin(radian);
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 1;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = std::sin(radian);
	result.m[2][1] = 0;
	result.m[2][2] = std::cos(radian);
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}

Matrix4x4 MakeRotatedZ(const float radian) {
	Matrix4x4 result;
	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1;
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}

Matrix4x4 MakeTranslation(const Vector3& translation) {
	Matrix4x4 result;
	result.m[0][0] = 1;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 1;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1;
	result.m[2][3] = 0;

	result.m[3][0] = translation.x;
	result.m[3][1] = translation.y;
	result.m[3][2] = translation.z;
	result.m[3][3] = 1;

	return result;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translation) {
	Matrix4x4 rotatedX = MakeRotatedX(rotate.x);
	Matrix4x4 rotatedY = MakeRotatedY(rotate.y);
	Matrix4x4 rotatedZ = MakeRotatedZ(rotate.z);

	Matrix4x4 rotatedXYZMatrix = MakeRotatedXYZ(rotatedX, rotatedY, rotatedZ);

	Matrix4x4 result;
	result.m[0][0] = scale.x * rotatedXYZMatrix.m[0][0];
	result.m[0][1] = scale.x * rotatedXYZMatrix.m[0][1];
	result.m[0][2] = scale.x * rotatedXYZMatrix.m[0][2];
	result.m[0][3] = 0;

	result.m[1][0] = scale.y * rotatedXYZMatrix.m[1][0];
	result.m[1][1] = scale.y * rotatedXYZMatrix.m[1][1];
	result.m[1][2] = scale.y * rotatedXYZMatrix.m[1][2];
	result.m[1][3] = 0;

	result.m[2][0] = scale.z * rotatedXYZMatrix.m[2][0];
	result.m[2][1] = scale.z * rotatedXYZMatrix.m[2][1];
	result.m[2][2] = scale.z * rotatedXYZMatrix.m[2][2];
	result.m[2][3] = 0;

	result.m[3][0] = translation.x;
	result.m[3][1] = translation.y;
	result.m[3][2] = translation.z;
	result.m[3][3] = 1;

	return result;
}

bool isCollisionAABB(const AABB& a, const AABB& b) {
	if (a.min.x <= b.max.x && a.max.x >= b.min.x &&
		a.min.y <= b.max.y && a.max.y >= b.min.y &&
		a.min.z <= b.max.z && a.max.z >= b.min.z) {
		return true;
	}
	return false;
}

bool IsCollision(const AABB& aabb, const Line& line) {
	float tMin = 0.0f;
	float tMax = 1.0f;

	// X軸
	if (line.diff.x == 0.0f) {
		if (line.origin.x < aabb.min.x || line.origin.x > aabb.max.x) {
			return false;
		}
	}
	else {
		float t1 = (aabb.min.x - line.origin.x) / line.diff.x;
		float t2 = (aabb.max.x - line.origin.x) / line.diff.x;

		float tNear = (std::min)(t1, t2);
		float tFar = (std::max)(t1, t2);

		tMin = (std::max)(tMin, tNear);
		tMax = (std::min)(tMax, tFar);

		if (tMin > tMax) {
			return false;
		}
	}
	// Y軸
	if (line.diff.y == 0.0f) {
		if (line.origin.y < aabb.min.y || line.origin.y > aabb.max.y) {
			return false;
		}
	}
	else {
		float t1 = (aabb.min.y - line.origin.y) / line.diff.y;
		float t2 = (aabb.max.y - line.origin.y) / line.diff.y;

		float tNear = (std::min)(t1, t2);
		float tFar = (std::max)(t1, t2);

		tMin = (std::max)(tMin, tNear);
		tMax = (std::min)(tMax, tFar);

		if (tMin > tMax) {
			return false;
		}
	}

	// Z軸
	if (line.diff.z == 0.0f) {
		if (line.origin.z < aabb.min.z || line.origin.z > aabb.max.z) {
			return false;
		}
	}
	else {
		float t1 = (aabb.min.z - line.origin.z) / line.diff.z;
		float t2 = (aabb.max.z - line.origin.z) / line.diff.z;

		float tNear = (std::min)(t1, t2);
		float tFar = (std::max)(t1, t2);

		tMin = (std::max)(tMin, tNear);
		tMax = (std::min)(tMax, tFar);

		if (tMin > tMax) {
			return false;
		}
	}

	return true;
}



//AABB GetAABB() {
//	AABB aabb;
//	return aabb;
//}
