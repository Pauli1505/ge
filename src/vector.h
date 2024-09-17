#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>

// Define a 3D vector
struct vec3f {
    float x, y, z;
};

// Vector operations
inline vec3f vec3f_add(const vec3f& a, const vec3f& b) {
    return vec3f{a.x + b.x, a.y + b.y, a.z + b.z};
}

inline vec3f vec3f_subtract(const vec3f& a, const vec3f& b) {
    return vec3f{a.x - b.x, a.y - b.y, a.z - b.z};
}

inline vec3f vec3f_scale(const vec3f& v, float s) {
    return vec3f{v.x * s, v.y * s, v.z * s};
}

inline vec3f vec3f_normalize(const vec3f& v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return vec3f{v.x / length, v.y / length, v.z / length};
}

inline vec3f vec3f_cross(const vec3f& a, const vec3f& b) {
    return vec3f{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// Dot product of two vectors
inline float vec3f_dot(const vec3f& a, const vec3f& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

#endif // VECTOR_H
