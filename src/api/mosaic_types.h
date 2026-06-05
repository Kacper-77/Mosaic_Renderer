#pragma once

#include <cstdint>

struct Vector4 {
    float x, y, z, w;
};

/* Vertex Layout */
struct Vertex {
    Vector4 position;
    uint32_t color; // Format: 0xAARRGGBB
};

struct Matrix4 {
    float m[4][4] = { { 0 } };

    static Matrix4 Identity() {
        Matrix4 mat;
        mat.m[0][0] = 1.0f; mat.m[1][1] = 1.0f; mat.m[2][2] = 1.0f; mat.m[3][3] = 1.0f;
        return mat;
    }

    Vector4 Multiply(const Vector4& v) const {
        return {
            v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0],
            v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1],
            v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2],
            v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3]
        };
    }

    static Matrix4 RotateY(float angleRadians) {
        Matrix4 mat = Identity();
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);
        mat.m[0][0] = c;  mat.m[2][0] = s;
        mat.m[0][2] = -s; mat.m[2][2] = c;
        return mat;
    }
};
