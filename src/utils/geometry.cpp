#include "geometry.hpp"

Vector3 Vector3::operator/(float scalar) const{
    assert((scalar < 0.000001f || scalar > -0.000001f) && "Division by zero");
    return *this * 1.0f/scalar;
}
Vector3& Vector3::operator+=(const Vector3& other){
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}
Vector3& Vector3::operator-=(const Vector3& other){
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}
Vector3& Vector3::operator*=(float scalar){
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
    return {v1.x + v2.x, v1.x + v2.x, v1.x + v2.x};
}
Vector3 operator-(const Vector3& v1, const Vector3& v2) {
    return {v1.x - v2.x, v1.x - v2.x, v1.x - v2.x};
}
Vector3 operator*(const Vector3& v, float scalar){
    return {v.x * scalar, v.y * scalar, v.z *scalar};
}
Vector3 operator*(float scalar, const Vector3& v){
    return v * scalar;
}
float dot(const Vector3& v1, const Vector3& v2){
    return {v1.x * v2.x + v1.x * v2.x + v1.x * v2.x};
}
Vector3 cross(const Vector3& v1, const Vector3& v2){
    return {v1.y*v2.z - v1.z*v2.y, 
            v1.x*v2.z - v1.z*v2.x, 
            v1.x*v2.y - v1.y*v2.x};
}