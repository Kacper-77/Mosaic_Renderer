#pragma once

#include <cstdint>

struct Vector3 {
    float x, y, z;

    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float _x, float _y, float _z) : x{_x}, y{_y}, z{_z} {}

    Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
    float Dot(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; }
    float Length() const { return std::sqrt(x * x + y * y + z * z); }

    Vector3 Normalized() const {
        float len = Length();
        if (len > 0.00001f) {
            float invLen = 1.0f / len;
            return Vector3(x * invLen, y * invLen, z * invLen);
        }
        return Vector3(0.0f, 0.0f, 0.0f);
    }
};

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
            v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3],  // X
            v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3],  // Y
            v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + v.w * m[2][3],  // Z
            v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + v.w * m[3][3]   // W
        };
    }

    static Matrix4 RotateX(float angleRadians) {
        Matrix4 mat = Identity();               
        float c = std::cos(angleRadians);       
        float s = std::sin(angleRadians);       
        mat.m[1][1] = c; mat.m[2][1] = -s;
        mat.m[1][2] = s; mat.m[2][2] = c;
        return mat;
    }

    static Matrix4 RotateY(float angleRadians) {
        Matrix4 mat = Identity();
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);
        mat.m[0][0] = c;  mat.m[2][0] = s;
        mat.m[0][2] = -s; mat.m[2][2] = c;
        return mat;
    }

    static Matrix4 RotateZ(float angleRadians) {
        Matrix4 mat = Identity();
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);
        mat.m[0][0] = c; mat.m[1][0] = -s;
        mat.m[0][1] = s; mat.m[1][1] = c;
        return mat;
    }

    static Matrix4 Translate(float x, float y, float z) {
        Matrix4 mat = Identity();
        mat.m[0][3] = x;
        mat.m[1][3] = y;
        mat.m[2][3] = z;
        return mat;
    }

    static Matrix4 Perspective(float fovDegrees, float aspect, float nearPlane, float farPlane) {
        Matrix4 mat;
        std::memset(&mat, 0, sizeof(Matrix4));

        float fovRad = fovDegrees * (M_PI / 180.0f);
        float tanHalfFov = std::tan(fovRad / 2.0f);

        mat.m[0][0] = 1.0f / (aspect * tanHalfFov);
        mat.m[1][1] = 1.0f / tanHalfFov;
        mat.m[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        mat.m[2][3] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        
        mat.m[3][2] = -1.0f; 
        
        return mat;
    }
    
    Matrix4 operator*(const Matrix4& other) const {
        Matrix4 res;
        // Row 0
        res.m[0][0] = m[0][0]*other.m[0][0] + m[0][1]*other.m[1][0] + m[0][2]*other.m[2][0] + m[0][3]*other.m[3][0];
        res.m[0][1] = m[0][0]*other.m[0][1] + m[0][1]*other.m[1][1] + m[0][2]*other.m[2][1] + m[0][3]*other.m[3][1];
        res.m[0][2] = m[0][0]*other.m[0][2] + m[0][1]*other.m[1][2] + m[0][2]*other.m[2][2] + m[0][3]*other.m[3][2];
        res.m[0][3] = m[0][0]*other.m[0][3] + m[0][1]*other.m[1][3] + m[0][2]*other.m[2][3] + m[0][3]*other.m[3][3];

        // Row 1
        res.m[1][0] = m[1][0]*other.m[0][0] + m[1][1]*other.m[1][0] + m[1][2]*other.m[2][0] + m[1][3]*other.m[3][0];
        res.m[1][1] = m[1][0]*other.m[0][1] + m[1][1]*other.m[1][1] + m[1][2]*other.m[2][1] + m[1][3]*other.m[3][1];
        res.m[1][2] = m[1][0]*other.m[0][2] + m[1][1]*other.m[1][2] + m[1][2]*other.m[2][2] + m[1][3]*other.m[3][2];
        res.m[1][3] = m[1][0]*other.m[0][3] + m[1][1]*other.m[1][3] + m[1][2]*other.m[2][3] + m[1][3]*other.m[3][3];

        // Row 2
        res.m[2][0] = m[2][0]*other.m[0][0] + m[2][1]*other.m[1][0] + m[2][2]*other.m[2][0] + m[2][3]*other.m[3][0];
        res.m[2][1] = m[2][0]*other.m[0][1] + m[2][1]*other.m[1][1] + m[2][2]*other.m[2][1] + m[2][3]*other.m[3][1];
        res.m[2][2] = m[2][0]*other.m[0][2] + m[2][1]*other.m[1][2] + m[2][2]*other.m[2][2] + m[2][3]*other.m[3][2];
        res.m[2][3] = m[2][0]*other.m[0][3] + m[2][1]*other.m[1][3] + m[2][2]*other.m[2][3] + m[2][3]*other.m[3][3];

        // Row 3
        res.m[3][0] = m[3][0]*other.m[0][0] + m[3][1]*other.m[1][0] + m[3][2]*other.m[2][0] + m[3][3]*other.m[3][0];
        res.m[3][1] = m[3][0]*other.m[0][1] + m[3][1]*other.m[1][1] + m[3][2]*other.m[2][1] + m[3][3]*other.m[3][1];
        res.m[3][2] = m[3][0]*other.m[0][2] + m[3][1]*other.m[1][2] + m[3][2]*other.m[2][2] + m[3][3]*other.m[3][2];
        res.m[3][3] = m[3][0]*other.m[0][3] + m[3][1]*other.m[1][3] + m[3][2]*other.m[2][3] + m[3][3]*other.m[3][3];

        return res;
    }
};
