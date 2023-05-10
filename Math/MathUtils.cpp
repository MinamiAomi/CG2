#include "MathUtils.h"
#include <cassert>

float Determinant(const Matrix4x4& m) {
	float result = 0.0f;

	result += m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]; // +11,22,33,44
	result += m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]; // +11,23,34,42
	result += m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]; // +11,24,32,43

	result -= m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]; // -11,24,33,42
	result -= m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]; // -11,23,32,44
	result -= m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]; // -11,22,34,43

	result -= m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]; // -12,21,33,44
	result -= m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]; // -13,21,34,42
	result -= m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]; // -14,21,32,43

	result += m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]; // +14,21,33,42
	result += m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]; // +13,21,32,44
	result += m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]; // +12,21,34,43

	result += m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]; // +12,23,31,44
	result += m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]; // +13,24,31,42
	result += m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]; // +14,22,31,43

	result -= m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]; // -14,23,31,42
	result -= m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]; // -13,22,31,44
	result -= m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]; // -12,24,31,43

	result -= m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]; // -12,23,34,41
	result -= m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]; // -13,24,32,41
	result -= m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]; // -14,22,33,41

	result += m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]; // +14,23,32,41
	result += m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]; // +13,22,34,41
	result += m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0]; // +12,24,33,41

	return result;
}

Matrix4x4 Adjugate(const Matrix4x4& m) {
	Matrix4x4 result = {};

	// 1
	result.m[0][0] = 0.0f;                               // 11
	result.m[0][0] += m.m[1][1] * m.m[2][2] * m.m[3][3]; // +22,33,44
	result.m[0][0] += m.m[1][2] * m.m[2][3] * m.m[3][1]; // +23,34,42
	result.m[0][0] += m.m[1][3] * m.m[2][1] * m.m[3][2]; // +24,32,43
	result.m[0][0] -= m.m[1][3] * m.m[2][2] * m.m[3][1]; // -24,33,42
	result.m[0][0] -= m.m[1][2] * m.m[2][1] * m.m[3][3]; // -23,32,44
	result.m[0][0] -= m.m[1][1] * m.m[2][3] * m.m[3][2]; // -22,34,43

	result.m[0][1] = 0.0f;                               // 12
	result.m[0][1] -= m.m[0][1] * m.m[2][2] * m.m[3][3]; // -12,33,44
	result.m[0][1] -= m.m[0][2] * m.m[2][3] * m.m[3][1]; // -13,34,42
	result.m[0][1] -= m.m[0][3] * m.m[2][1] * m.m[3][2]; // -14,32,43
	result.m[0][1] += m.m[0][3] * m.m[2][2] * m.m[3][1]; // +14,33,42
	result.m[0][1] += m.m[0][2] * m.m[2][1] * m.m[3][3]; // +13,32,44
	result.m[0][1] += m.m[0][1] * m.m[2][3] * m.m[3][2]; // +12,34,43

	result.m[0][2] = 0.0f;                               // 13
	result.m[0][2] += m.m[0][1] * m.m[1][2] * m.m[3][3]; // +12,23,44
	result.m[0][2] += m.m[0][2] * m.m[1][3] * m.m[3][1]; // +13,24,42
	result.m[0][2] += m.m[0][3] * m.m[1][1] * m.m[3][2]; // +14,22,43
	result.m[0][2] -= m.m[0][3] * m.m[1][2] * m.m[3][1]; // -14,23,42
	result.m[0][2] -= m.m[0][2] * m.m[1][1] * m.m[3][3]; // -13,22,44
	result.m[0][2] -= m.m[0][1] * m.m[1][3] * m.m[3][2]; // -12,24,43

	result.m[0][3] = 0.0f;                               // 14
	result.m[0][3] -= m.m[0][1] * m.m[1][2] * m.m[2][3]; // -12,23,34
	result.m[0][3] -= m.m[0][2] * m.m[1][3] * m.m[2][1]; // -13,24,32
	result.m[0][3] -= m.m[0][3] * m.m[1][1] * m.m[2][2]; // -14,22,33
	result.m[0][3] += m.m[0][3] * m.m[1][2] * m.m[2][1]; // +14,23,32
	result.m[0][3] += m.m[0][2] * m.m[1][1] * m.m[2][3]; // +13,22,34
	result.m[0][3] += m.m[0][1] * m.m[1][3] * m.m[2][2]; // +12,24,33

	// 2
	result.m[1][0] = 0.0f;                               // 21
	result.m[1][0] -= m.m[1][0] * m.m[2][2] * m.m[3][3]; // -21,33,44
	result.m[1][0] -= m.m[1][2] * m.m[2][3] * m.m[3][0]; // -23,34,41
	result.m[1][0] -= m.m[1][3] * m.m[2][0] * m.m[3][2]; // -24,31,43
	result.m[1][0] += m.m[1][3] * m.m[2][2] * m.m[3][0]; // +24,33,41
	result.m[1][0] += m.m[1][2] * m.m[2][0] * m.m[3][3]; // +23,31,44
	result.m[1][0] += m.m[1][0] * m.m[2][3] * m.m[3][2]; // +21,34,43

	result.m[1][1] = 0.0f;                               // 22
	result.m[1][1] += m.m[0][0] * m.m[2][2] * m.m[3][3]; // +11,33,44
	result.m[1][1] += m.m[0][2] * m.m[2][3] * m.m[3][0]; // +13,34,41
	result.m[1][1] += m.m[0][3] * m.m[2][0] * m.m[3][2]; // +14,31,43
	result.m[1][1] -= m.m[0][3] * m.m[2][2] * m.m[3][0]; // -14,33,41
	result.m[1][1] -= m.m[0][2] * m.m[2][0] * m.m[3][3]; // -13,31,44
	result.m[1][1] -= m.m[0][0] * m.m[2][3] * m.m[3][2]; // -11,34,43

	result.m[1][2] = 0.0f;                               // 23
	result.m[1][2] -= m.m[0][0] * m.m[1][2] * m.m[3][3]; // -11,23,44
	result.m[1][2] -= m.m[0][2] * m.m[1][3] * m.m[3][0]; // -13,24,41
	result.m[1][2] -= m.m[0][3] * m.m[1][0] * m.m[3][2]; // -14,21,43
	result.m[1][2] += m.m[0][3] * m.m[1][2] * m.m[3][0]; // +14,23,41
	result.m[1][2] += m.m[0][2] * m.m[1][0] * m.m[3][3]; // +13,21,44
	result.m[1][2] += m.m[0][0] * m.m[1][3] * m.m[3][2]; // +11,24,43

	result.m[1][3] = 0.0f;                               // 24
	result.m[1][3] += m.m[0][0] * m.m[1][2] * m.m[2][3]; // +11,23,34
	result.m[1][3] += m.m[0][2] * m.m[1][3] * m.m[2][0]; // +13,24,31
	result.m[1][3] += m.m[0][3] * m.m[1][0] * m.m[2][2]; // +14,21,33
	result.m[1][3] -= m.m[0][3] * m.m[1][2] * m.m[2][0]; // -14,23,31
	result.m[1][3] -= m.m[0][2] * m.m[1][0] * m.m[2][3]; // -13,21,34
	result.m[1][3] -= m.m[0][0] * m.m[1][3] * m.m[2][2]; // -11,24,33

	// 3
	result.m[2][0] = 0.0f;                               // 31
	result.m[2][0] += m.m[1][0] * m.m[2][1] * m.m[3][3]; // +21,32,44
	result.m[2][0] += m.m[1][1] * m.m[2][3] * m.m[3][0]; // +22,34,41
	result.m[2][0] += m.m[1][3] * m.m[2][0] * m.m[3][1]; // +24,31,42
	result.m[2][0] -= m.m[1][3] * m.m[2][1] * m.m[3][0]; // -24,32,41
	result.m[2][0] -= m.m[1][1] * m.m[2][0] * m.m[3][3]; // -22,31,44
	result.m[2][0] -= m.m[1][0] * m.m[2][3] * m.m[3][1]; // -21,34,42

	result.m[2][1] = 0.0f;                               // 32
	result.m[2][1] -= m.m[0][0] * m.m[2][1] * m.m[3][3]; // -11,32,44
	result.m[2][1] -= m.m[0][1] * m.m[2][3] * m.m[3][0]; // -12,34,41
	result.m[2][1] -= m.m[0][3] * m.m[2][0] * m.m[3][1]; // -14,31,42
	result.m[2][1] += m.m[0][3] * m.m[2][1] * m.m[3][0]; // +14,32,41
	result.m[2][1] += m.m[0][1] * m.m[2][0] * m.m[3][3]; // +12,31,44
	result.m[2][1] += m.m[0][0] * m.m[2][3] * m.m[3][1]; // +11,34,42

	result.m[2][2] = 0.0f;                               // 33
	result.m[2][2] += m.m[0][0] * m.m[1][1] * m.m[3][3]; // +11,22,44
	result.m[2][2] += m.m[0][1] * m.m[1][3] * m.m[3][0]; // +12,24,41
	result.m[2][2] += m.m[0][3] * m.m[1][0] * m.m[3][1]; // +14,21,42
	result.m[2][2] -= m.m[0][3] * m.m[1][1] * m.m[3][0]; // -14,22,41
	result.m[2][2] -= m.m[0][1] * m.m[1][0] * m.m[3][3]; // -12,21,44
	result.m[2][2] -= m.m[0][0] * m.m[1][3] * m.m[3][1]; // -11,24,42

	result.m[2][3] = 0.0f;                               // 34
	result.m[2][3] -= m.m[0][0] * m.m[1][1] * m.m[2][3]; // -11,22,34
	result.m[2][3] -= m.m[0][1] * m.m[1][3] * m.m[2][0]; // -12,24,31
	result.m[2][3] -= m.m[0][3] * m.m[1][0] * m.m[2][1]; // -14,21,32
	result.m[2][3] += m.m[0][3] * m.m[1][1] * m.m[2][0]; // +14,22,31
	result.m[2][3] += m.m[0][1] * m.m[1][0] * m.m[2][3]; // +12,21,34
	result.m[2][3] += m.m[0][0] * m.m[1][3] * m.m[2][1]; // +11,24,32

	// 4
	result.m[3][0] = 0.0f;                               // 41
	result.m[3][0] -= m.m[1][0] * m.m[2][1] * m.m[3][2]; // -21,32,43
	result.m[3][0] -= m.m[1][1] * m.m[2][2] * m.m[3][0]; // -22,33,41
	result.m[3][0] -= m.m[1][2] * m.m[2][0] * m.m[3][1]; // -23,31,42
	result.m[3][0] += m.m[1][2] * m.m[2][1] * m.m[3][0]; // +23,32,41
	result.m[3][0] += m.m[1][1] * m.m[2][0] * m.m[3][2]; // +22,31,43
	result.m[3][0] += m.m[1][0] * m.m[2][2] * m.m[3][1]; // +21,33,42

	result.m[3][1] = 0.0f;                               // 42
	result.m[3][1] += m.m[0][0] * m.m[2][1] * m.m[3][2]; // +11,32,43
	result.m[3][1] += m.m[0][1] * m.m[2][2] * m.m[3][0]; // +12,33,41
	result.m[3][1] += m.m[0][2] * m.m[2][0] * m.m[3][1]; // +13,31,42
	result.m[3][1] -= m.m[0][2] * m.m[2][1] * m.m[3][0]; // -13,32,41
	result.m[3][1] -= m.m[0][1] * m.m[2][0] * m.m[3][2]; // -12,31,43
	result.m[3][1] -= m.m[0][0] * m.m[2][2] * m.m[3][1]; // -11,33,42

	result.m[3][2] = 0.0f;                               // 43
	result.m[3][2] -= m.m[0][0] * m.m[1][1] * m.m[3][2]; // -11,22,43
	result.m[3][2] -= m.m[0][1] * m.m[1][2] * m.m[3][0]; // -12,23,41
	result.m[3][2] -= m.m[0][2] * m.m[1][0] * m.m[3][1]; // -13,21,42
	result.m[3][2] += m.m[0][2] * m.m[1][1] * m.m[3][0]; // +13,22,41
	result.m[3][2] += m.m[0][1] * m.m[1][0] * m.m[3][2]; // +12,21,43
	result.m[3][2] += m.m[0][0] * m.m[1][2] * m.m[3][1]; // +11,23,42

	result.m[3][3] = 0.0f;                               // 44
	result.m[3][3] += m.m[0][0] * m.m[1][1] * m.m[2][2]; // +11,22,33
	result.m[3][3] += m.m[0][1] * m.m[1][2] * m.m[2][0]; // +12,23,31
	result.m[3][3] += m.m[0][2] * m.m[1][0] * m.m[2][1]; // +13,21,32
	result.m[3][3] -= m.m[0][2] * m.m[1][1] * m.m[2][0]; // -13,22,31
	result.m[3][3] -= m.m[0][1] * m.m[1][0] * m.m[2][2]; // -12,21,33
	result.m[3][3] -= m.m[0][0] * m.m[1][2] * m.m[2][1]; // -11,23,32

	return result;
}

Matrix4x4 Inverse(const Matrix4x4& m) { return 1.0f / Determinant(m) * Adjugate(m); }

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& trans) {
	Matrix4x4 rot = MakeIdentityMatrix();
	rot *= MakeRotateXMatrix(rotate.x);
	rot *= MakeRotateYMatrix(rotate.y);
	rot *= MakeRotateZMatrix(rotate.z);

	Matrix4x4 mat = {};
	mat.m[0][0] = scale.x * rot.m[0][0];
	mat.m[0][1] = scale.x * rot.m[0][1];
	mat.m[0][2] = scale.x * rot.m[0][2];
	mat.m[0][3] = 0.0f;

	mat.m[1][0] = scale.y * rot.m[1][0];
	mat.m[1][1] = scale.y * rot.m[1][1];
	mat.m[1][2] = scale.y * rot.m[1][2];
	mat.m[1][3] = 0.0f;

	mat.m[2][0] = scale.z * rot.m[2][0];
	mat.m[2][1] = scale.z * rot.m[2][1];
	mat.m[2][2] = scale.z * rot.m[2][2];
	mat.m[2][3] = 0.0f;

	mat.m[3][0] = trans.x;
	mat.m[3][1] = trans.y;
	mat.m[3][2] = trans.z;
	mat.m[3][3] = 1.0f;

	return mat;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspect, float nearZ, float farZ) {
	float cotFov = 1.0f / std::tanf(fovY * 0.5f);
	float farDivZLen = farZ / (farZ - nearZ);
	return {
	    cotFov / aspect,
	    0.0f,
	    0.0f,
	    0.0f,
	    0.0f,
	    cotFov,
	    0.0f,
	    0.0f,
	    0.0f,
	    0.0f,
	    farDivZLen,
	    1.0f,
	    0.0f,
	    0.0f,
	    farDivZLen * -nearZ,
	    0.0f};
}

Matrix4x4 MakeOrthographicMatrix(
    float left, float top, float right, float bottom, float nearZ, float farZ) {
	float width = right - left;
	float height = top - bottom;
	float zLen = farZ - nearZ;
	return {2.0f / width,
	        0.0f,
	        0.0f,
	        0.0f,
	        0.0f,
	        2.0f / height,
	        0.0f,
	        0.0f,
	        0.0f,
	        0.0f,
	        1.0f / zLen,
	        0.0f,
	        (left + right) / -width,
	        (top + bottom) / -height,
	        nearZ / -zLen,
	        1.0f};
}

Matrix4x4 MakeViewportMatrix(
    float left, float top, float width, float height, float minDepth, float maxDepth) {
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;
	return {
	    halfWidth,
	    0.0f,
	    0.0f,
	    0.0f,
	    0.0f,
	    -halfHeight,
	    0.0f,
	    0.0f,
	    0.0f,
	    0.0f,
	    maxDepth - minDepth,
	    0.0f,
	    left + halfWidth,
	    top + halfHeight,
	    minDepth,
	    1.0f};
}

Vector3 Math::Transform(const Vector3& v, const Matrix4x4& m) {
	Vector3 result = {};
	result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + 1.0f * m.m[3][0];
	result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + 1.0f * m.m[3][1];
	result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + 1.0f * m.m[3][2];
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + 1.0f * m.m[3][3];
	assert(w != 0.0f);
	result *= 1.0f / w;
	return result;
}

Vector3 Math::TransformNormal(const Vector3& v, const Matrix4x4& m) {
	return {
	    v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
	    v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
	    v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]};
}

Vector2 Slerp(float t, const Vector2& s, const Vector2& e) {
	float dot = Dot(Normalize(s), Normalize(e));
	if (std::abs(dot) > 0.999f) {
		return Lerp(t, s, e);
	}
	float theta = std::acos(dot);
	float sinTheta = std::sin(theta);
	float t1 = std::sin((1.0f - t) * theta) / sinTheta;
	float t2 = std::sin(t * theta) / sinTheta;

	return t1 * s + t2 * e;
}

Vector3 Slerp(float t, const Vector3& s, const Vector3& e) {
	float dot = Dot(Normalize(s), Normalize(e));
	if (std::abs(dot) > 0.999f) {
		return Lerp(t, s, e);
	}
	float theta = std::acos(dot);
	float sinTheta = std::sin(theta);
	float t1 = std::sin((1.0f - t) * theta) / sinTheta;
	float t2 = std::sin(t * theta) / sinTheta;

	return t1 * s + t2 * e;
}