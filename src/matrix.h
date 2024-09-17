#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>

constexpr float pi = 3.141593f;

struct mat4f {
    // Column-major order
    float
        x11, x21, x31, x41,
        x12, x22, x32, x42,
        x13, x23, x33, x43,
        x14, x24, x34, x44;
};

float* mat4f_gl(mat4f* m) {
    // since its already in the column major, just return the
    // first element.
    return &m->x11;
}

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

inline mat4f mat4f_perspective() {
    const float
        r = 0.5f,  // Half of the viewport width (at the near plane)
        t = 0.5f,  // Half of the viewport height (at the near plane)
        n = 1.0f,  // Distance to near clipping plane
        f = 5.0f;  // Distance to far clipping plane

    return mat4f{
        n / r, 0, 0, 0,
        0, n / t, 0, 0,
        0, 0, (-f - n) / (f - n), -1,
        0, 0, (2 * f * n) / (n - f), 0,
    };
}

#endif

