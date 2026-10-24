#include "Function.h"


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

Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

Vector3 MultiplyByS(const Vector3& v, float scalar) {
	return { v.x * scalar, v.y * scalar, v.z * scalar };
}

Vector3 Multiply2Vectors(const Vector3& v1, const Vector3& v2) {
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
	return MultiplyByS(v, 1.0f / length);
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

		Vector3 p = MultiplyByS(v2, t);
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

//Matrix4x4 MakeRotatedXYZ(const Vector3& rotation) {
//
//}

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