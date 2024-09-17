// matrix.h
#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
#include "vector.h"

constexpr float pi = 3.141593f;

struct mat4f {
    // Column-major order
    float
        x11, x21, x31, x41,
        x12, x22, x32, x42,
        x13, x23, x33, x43,
        x14, x24, x34, x44;
};

// Declaration of mat4f_gl
float* mat4f_gl(mat4f* m);

mat4f mat4f_look_at(const vec3f& position, const vec3f& target, const vec3f& up);
mat4f mat4f_perspective(float fov, float aspect, float near, float far);

// Inline functions...
inline mat4f mat4f_identity() {
    return mat4f{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
}

inline mat4f mat4f_multiply(const mat4f& a, const mat4f& b) {
    return mat4f{
        a.x11 * b.x11 + a.x12 * b.x21 + a.x13 * b.x31 + a.x14 * b.x41,
        a.x21 * b.x11 + a.x22 * b.x21 + a.x23 * b.x31 + a.x24 * b.x41,
        a.x31 * b.x11 + a.x32 * b.x21 + a.x33 * b.x31 + a.x34 * b.x41,
        a.x41 * b.x11 + a.x42 * b.x21 + a.x43 * b.x31 + a.x44 * b.x41,

        a.x11 * b.x12 + a.x12 * b.x22 + a.x13 * b.x32 + a.x14 * b.x42,
        a.x21 * b.x12 + a.x22 * b.x22 + a.x23 * b.x32 + a.x24 * b.x42,
        a.x31 * b.x12 + a.x32 * b.x22 + a.x33 * b.x32 + a.x34 * b.x42,
        a.x41 * b.x12 + a.x42 * b.x22 + a.x43 * b.x32 + a.x44 * b.x42,

        a.x11 * b.x13 + a.x12 * b.x23 + a.x13 * b.x33 + a.x14 * b.x43,
        a.x21 * b.x13 + a.x22 * b.x23 + a.x23 * b.x33 + a.x24 * b.x43,
        a.x31 * b.x13 + a.x32 * b.x23 + a.x33 * b.x33 + a.x34 * b.x43,
        a.x41 * b.x13 + a.x42 * b.x23 + a.x43 * b.x33 + a.x44 * b.x43,

        a.x11 * b.x14 + a.x12 * b.x24 + a.x13 * b.x34 + a.x14 * b.x44,
        a.x21 * b.x14 + a.x22 * b.x24 + a.x23 * b.x34 + a.x24 * b.x44,
        a.x31 * b.x14 + a.x32 * b.x24 + a.x33 * b.x34 + a.x34 * b.x44,
        a.x41 * b.x14 + a.x42 * b.x24 + a.x43 * b.x34 + a.x44 * b.x44,
    };
}

inline mat4f mat4f_scale(float x, float y, float z) {
    return mat4f{
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1,
    };
}

inline mat4f mat4f_translation(float x, float y, float z) {
    return mat4f{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1,
    };
}

inline mat4f mat4f_rotate_z(float theta) {
    return mat4f{
        std::cos(theta), std::sin(theta), 0, 0,
        -std::sin(theta), std::cos(theta), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
}

inline mat4f mat4f_rotate_y(float theta) {
    return mat4f{
        std::cos(theta), 0, -std::sin(theta), 0,
        0, 1, 0, 0,
        std::sin(theta), 0, std::cos(theta), 0,
        0, 0, 0, 1,
    };
}

inline mat4f mat4f_rotate_x(float theta) {
    return mat4f{
        1, 0, 0, 0,
        0, std::cos(theta), std::sin(theta), 0,
        0, -std::sin(theta), std::cos(theta), 0,
        0, 0, 0, 1,
    };
}

// Function to create a perspective projection matrix
inline mat4f mat4f_perspective(float fov, float aspect, float near, float far) {
    float tanHalfFov = tanf(fov / 2.0f * pi / 180.0f);
    
    return mat4f{
        1.0f / (aspect * tanHalfFov), 0, 0, 0,
        0, 1.0f / tanHalfFov, 0, 0,
        0, 0, -(far + near) / (far - near), -1.0f,
        0, 0, -2.0f * far * near / (far - near), 0
    };
}

// Function to create a look-at matrix
inline mat4f mat4f_look_at(const vec3f& position, const vec3f& target, const vec3f& up) {
    vec3f f = vec3f_normalize(vec3f_subtract(target, position));
    vec3f r = vec3f_normalize(vec3f_cross(up, f));
    vec3f u = vec3f_cross(f, r);

    return mat4f{
        r.x, u.x, -f.x, 0,
        r.y, u.y, -f.y, 0,
        r.z, u.z, -f.z, 0,
        -vec3f_dot(r, position), -vec3f_dot(u, position), vec3f_dot(f, position), 1
    };
}

#endif // MATRIX_H
