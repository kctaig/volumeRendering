# pragma once

class Vec3 {
   public:
   Vec3(){}
    Vec3(float x, float y, float z)
        : x(x), y(y), z(z) {}
    Vec3 operator+(const Vec3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vec3 operator-(const Vec3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vec3 operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }

    float x, y, z;
};