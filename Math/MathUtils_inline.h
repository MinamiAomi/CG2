#include "MathUtils.h"
#pragma once

inline Vector2 operator+(const Vector2& v) { return {+v.x, +v.y}; }
inline Vector2 operator-(const Vector2& v) { return {-v.x, -v.y}; }
inline Vector2 operator+(const Vector2& v1, const Vector2& v2) {
	return {v1.x + v2.x, v1.y + v2.y};
}
inline Vector2 operator-(const Vector2& v1, const Vector2& v2) {
	return {v1.x - v2.x, v1.y - v2.y};
}
inline Vector2 operator*(const Vector2& v, float s) { return {v.x * s, v.y * s}; }
inline Vector2 operator*(float s, const Vector2& v) { return {s * v.x, s * v.y}; }
inline Vector2& operator+=(Vector2& v1, const Vector2& v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	return v1;
}
inline Vector2& operator-=(Vector2& v1, const Vector2& v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	return v1;
}
inline Vector2& operator*=(Vector2& v, float s) {
	v.x *= s;
	v.y *= s;
	return v;
}
inline bool operator==(const Vector2& v1, const Vector2& v2) {
	return v1.x == v2.x && v1.y == v2.y;
}
inline bool operator!=(const Vector2& v1, const Vector2& v2) {
	return v1.x != v2.x || v1.y != v2.y;
}
inline float Dot(const Vector2& v1, const Vector2& v2) { return v1.x * v2.x + v1.y * v2.y; }
inline float Cross(const Vector2& v1, const Vector2& v2) { return v1.x * v2.y - v1.y * v2.x; }
inline float LengthSquare(const Vector2& v) { return Dot(v, v); }
inline float Length(const Vector2& v) { return std::sqrt(LengthSquare(v)); }
inline Vector2 Normalize(const Vector2& v) { return v * (1.0f / Length(v)); }

inline Vector3 operator+(const Vector3& v) { return {+v.x, +v.y, +v.z}; }
inline Vector3 operator-(const Vector3& v) { return {-v.x, -v.y, -v.z}; }
inline Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}
inline Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}
inline Vector3 operator*(const Vector3& v, float s) { return {v.x * s, v.y * s, v.z * s}; }
inline Vector3 operator*(float s, const Vector3& v) { return {s * v.x, s * v.y, s * v.z}; }
inline Vector3& operator+=(Vector3& v1, const Vector3& v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}
inline Vector3& operator-=(Vector3& v1, const Vector3& v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	return v1;
}
inline Vector3& operator*=(Vector3& v, float s) {
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}
inline bool operator==(const Vector3& v1, const Vector3& v2) {
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}
inline bool operator!=(const Vector3& v1, const Vector3& v2) {
	return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
}
inline float Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
inline Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
}
inline float LengthSquare(const Vector3& v) { return Dot(v, v); }
inline float Length(const Vector3& v) { return std::sqrt(LengthSquare(v)); }
inline Vector3 Normalize(const Vector3& v) { return v * (1.0f / Length(v)); }

inline Vector3 ToVector3(const Vector4& v) {
	return { v.x,v.y,v.z };
}

inline Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) {
	return {m1.m[0][0] + m2.m[0][0], m1.m[0][1] + m2.m[0][1], m1.m[0][2] + m2.m[0][2],
	        m1.m[0][3] + m2.m[0][3], m1.m[1][0] + m2.m[1][0], m1.m[1][1] + m2.m[1][1],
	        m1.m[1][2] + m2.m[1][2], m1.m[1][3] + m2.m[1][3], m1.m[2][0] + m2.m[2][0],
	        m1.m[2][1] + m2.m[2][1], m1.m[2][2] + m2.m[2][2], m1.m[2][3] + m2.m[2][3],
	        m1.m[3][0] + m2.m[3][0], m1.m[3][1] + m2.m[3][1], m1.m[3][2] + m2.m[3][2],
	        m1.m[3][3] + m2.m[3][3]};
}
inline Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) {
	return {m1.m[0][0] - m2.m[0][0], m1.m[0][1] - m2.m[0][1], m1.m[0][2] - m2.m[0][2],
	        m1.m[0][3] - m2.m[0][3], m1.m[1][0] - m2.m[1][0], m1.m[1][1] - m2.m[1][1],
	        m1.m[1][2] - m2.m[1][2], m1.m[1][3] - m2.m[1][3], m1.m[2][0] - m2.m[2][0],
	        m1.m[2][1] - m2.m[2][1], m1.m[2][2] - m2.m[2][2], m1.m[2][3] - m2.m[2][3],
	        m1.m[3][0] - m2.m[3][0], m1.m[3][1] - m2.m[3][1], m1.m[3][2] - m2.m[3][2],
	        m1.m[3][3] - m2.m[3][3]};
}
inline Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
	return {
	    m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] +
	        m1.m[0][3] * m2.m[3][0],
	    m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] +
	        m1.m[0][3] * m2.m[3][1],
	    m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] +
	        m1.m[0][3] * m2.m[3][2],
	    m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] +
	        m1.m[0][3] * m2.m[3][3],

	    m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] +
	        m1.m[1][3] * m2.m[3][0],
	    m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] +
	        m1.m[1][3] * m2.m[3][1],
	    m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] +
	        m1.m[1][3] * m2.m[3][2],
	    m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] +
	        m1.m[1][3] * m2.m[3][3],

	    m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] +
	        m1.m[2][3] * m2.m[3][0],
	    m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] +
	        m1.m[2][3] * m2.m[3][1],
	    m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] +
	        m1.m[2][3] * m2.m[3][2],
	    m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] +
	        m1.m[2][3] * m2.m[3][3],

	    m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] +
	        m1.m[3][3] * m2.m[3][0],
	    m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] +
	        m1.m[3][3] * m2.m[3][1],
	    m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] +
	        m1.m[3][3] * m2.m[3][2],
	    m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] +
	        m1.m[3][3] * m2.m[3][3]};
}
inline Matrix4x4 operator*(const Matrix4x4& m, float s) {
	return {m.m[0][0] * s, m.m[0][1] * s, m.m[0][2] * s, m.m[0][3] * s,
	        m.m[1][0] * s, m.m[1][1] * s, m.m[1][2] * s, m.m[1][3] * s,
	        m.m[2][0] * s, m.m[2][1] * s, m.m[2][2] * s, m.m[2][3] * s,
	        m.m[3][0] * s, m.m[3][1] * s, m.m[3][2] * s, m.m[3][3] * s};
}
inline Matrix4x4 operator*(float s, const Matrix4x4& m) {
	return {s * m.m[0][0], s * m.m[0][1], s * m.m[0][2], s * m.m[0][3],
	        s * m.m[1][0], s * m.m[1][1], s * m.m[1][2], s * m.m[1][3],
	        s * m.m[2][0], s * m.m[2][1], s * m.m[2][2], s * m.m[2][3],
	        s * m.m[3][0], s * m.m[3][1], s * m.m[3][2], s * m.m[3][3]};
}
inline Matrix4x4 operator*=(Matrix4x4& m1, const Matrix4x4& m2) {
	m1 = m1 * m2;
	return m1;
}
inline Matrix4x4 Transpose(const Matrix4x4& m) {
	return {m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0], m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
	        m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2], m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]};
}
inline Matrix4x4 MakeIdentityMatrix() {
	return {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}
inline Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	return {scale.x, 0.0f, 0.0f,    0.0f, 0.0f, scale.y, 0.0f, 0.0f,
	        0.0f,    0.0f, scale.z, 0.0f, 0.0f, 0.0f,    0.0f, 1.0f};
}
inline Matrix4x4 MakeRotateXMatrix(float rad) {
	float s = std::sin(rad);
	float c = std::cos(rad);
	return {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c, s, 0.0f, 0.0f, -s, c, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}
inline Matrix4x4 MakeRotateYMatrix(float rad) {
	float s = std::sin(rad);
	float c = std::cos(rad);
	return {c, 0.0f, -s, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, s, 0.0f, c, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}
inline Matrix4x4 MakeRotateZMatrix(float rad) {
	float s = std::sin(rad);
	float c = std::cos(rad);
	return {c, s, 0.0f, 0.0f, -s, c, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}
inline Matrix4x4 MakeTranslateMatrix(const Vector3& trans) {
	return {1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    1.0f,    0.0f,    0.0f,
	        0.0f, 0.0f, 1.0f, 0.0f, trans.x, trans.y, trans.z, 1.0f};
}

inline Vector3 GetTranslate(const Matrix4x4& m) { return {m.m[3][0], m.m[3][1], m.m[3][2]}; }

inline Vector3 operator*(const Vector3& v, const Matrix4x4& m) {
	return {
	    v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0],
	    v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1],
	    v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]};
}

inline Vector2 Lerp(float t, const Vector2& s, const Vector2& e) { return s + t * (e - s); }
inline Vector3 Lerp(float t, const Vector3& s, const Vector3& e) { return s + t * (e - s); }

inline void Copy(const Vector2& in, float out[2]) { memcpy(out, &in.x, sizeof(float) * 2); }
inline void Copy(float in[2], Vector2& out) { memcpy(&out.x, in, sizeof(float) * 2); }
inline void Copy(const Vector3& in, float out[3]) { memcpy(out, &in.x, sizeof(float) * 3); }
inline void Copy(float in[3], Vector3& out) { memcpy(&out.x, in, sizeof(float) * 3); }