#pragma once

#include <cassert>

struct Vector3{
    float x{0.0f}, y{0.0f}, z{0.0f};

    Vector3 operator/(float scalar) const;
    Vector3& operator+=(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3& operator*=(float scalar);
};

Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator*(const Vector3& v1, float scalar);
Vector3 operator*(float scalar, const Vector3& v);
float dot(const Vector3& v1, const Vector3& v2);
Vector3 cross(const Vector3& v1, const Vector3& v2);
