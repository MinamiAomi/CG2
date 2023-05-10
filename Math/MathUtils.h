#pragma once
#include <cmath>
#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix4x4.h>
#include <MathUtils_inline.h>

namespace Math {
	constexpr float Pi = 3.141592653589793f;
	constexpr float TwoPi = Pi * 2.0f;
	constexpr float HalfPi = Pi * 0.5f;

	constexpr inline float ToRad(float deg) { return deg * Pi / 180.0f; }
	constexpr inline float ToDeg(float rad) { return rad * 180.0f / Pi; }

	Vector3 Transform(const Vector3& v, const Matrix4x4& m);
	Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);
} // namespace Math


inline Vector2 operator+(const Vector2& v);
inline Vector2 operator-(const Vector2& v);
inline Vector2 operator+(const Vector2& v1, const Vector2& v2);
inline Vector2 operator-(const Vector2& v1, const Vector2& v2);
inline Vector2 operator*(const Vector2& v, float s);
inline Vector2 operator*(float s, const Vector2& v);
inline Vector2& operator+=(Vector2& v1, const Vector2& v2);
inline Vector2& operator-=(Vector2& v1, const Vector2& v2);
inline Vector2& operator*=(Vector2& v, float s);
inline bool operator==(const Vector2& v1, const Vector2& v2);
inline bool operator!=(const Vector2& v1, const Vector2& v2);
inline float Dot(const Vector2& v1, const Vector2& v2);
inline float Cross(const Vector2& v1, const Vector2& v2);
inline float LengthSquare(const Vector2& v);
inline float Length(const Vector2& v);
inline Vector2 Normalize(const Vector2& v);
inline Vector3 operator+(const Vector3& v);
inline Vector3 operator-(const Vector3& v);
inline Vector3 operator+(const Vector3& v1, const Vector3& v2);
inline Vector3 operator-(const Vector3& v1, const Vector3& v2);
inline Vector3 operator*(const Vector3& v, float s);
inline Vector3 operator*(float s, const Vector3& v);
inline Vector3& operator+=(Vector3& v1, const Vector3& v2);
inline Vector3& operator-=(Vector3& v1, const Vector3& v2);
inline Vector3& operator*=(Vector3& v, float s);
inline bool operator==(const Vector3& v1, const Vector3& v2);
inline bool operator!=(const Vector3& v1, const Vector3& v2);
inline float Dot(const Vector3& v1, const Vector3& v2);
inline Vector3 Cross(const Vector3& v1, const Vector3& v2);
inline float LengthSquare(const Vector3& v);
inline float Length(const Vector3& v);
inline Vector3 Normalize(const Vector3& v);

inline Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2);
inline Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2);
inline Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
inline Matrix4x4 operator*(const Matrix4x4& m, float s);
inline Matrix4x4 operator*(float s, const Matrix4x4& m);
inline Matrix4x4 operator*=(Matrix4x4& m1, const Matrix4x4& m2);
inline Matrix4x4 Transpose(const Matrix4x4& m);
float Determinant(const Matrix4x4& m);
Matrix4x4 Adjugate(const Matrix4x4& m);
Matrix4x4 Inverse(const Matrix4x4& m);
inline Matrix4x4 MakeIdentityMatrix();
inline Matrix4x4 MakeScaleMatrix(const Vector3& scale);
inline Matrix4x4 MakeRotateXMatrix(float rad);
inline Matrix4x4 MakeRotateYMatrix(float rad);
inline Matrix4x4 MakeRotateZMatrix(float rad);
inline Matrix4x4 MakeTranslateMatrix(const Vector3& trans);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& trans);
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspect, float nearZ, float farZ);
Matrix4x4 MakeOrthographicMatrix(
	float left, float top, float right, float bottom, float nearZ, float farZ);
Matrix4x4 MakeViewportMatrix(
	float left, float top, float width, float height, float minDepth, float maxDepth);

inline Vector3 GetTranslate(const Matrix4x4& m);
inline Vector3 operator*(const Vector3& v, const Matrix4x4& m);

inline Vector2 Lerp(float t, const Vector2& s, const Vector2& e);
inline Vector3 Lerp(float t, const Vector3& s, const Vector3& e);
Vector2 Slerp(float t, const Vector2& s, const Vector2& e);
Vector3 Slerp(float t, const Vector3& s, const Vector3& e);

inline void Copy(const Vector2& in, float out[2]);
inline void Copy(float in[2], Vector2& out);
inline void Copy(const Vector3& in, float out[3]);
inline void Copy(float in[3], Vector3& out);

