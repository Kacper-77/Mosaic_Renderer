#pragma once

#include "../api/mosaic_types.h" 
#include <algorithm>
#include <cmath>

struct VertexColor {
    float r, g, b;
};

struct FragmentInput {
    float x, y, z;          
    float wInv;       
    Vector3 posWorld;      
    Vector3 normal;   
    VertexColor vertexColor;
};

namespace Shaders {

    inline uint32_t PackColor(const VertexColor& color) {
        float r = std::max(0.0f, std::min(1.0f, color.r));
        float g = std::max(0.0f, std::min(1.0f, color.g));
        float b = std::max(0.0f, std::min(1.0f, color.b));

        auto finalR = static_cast<uint8_t>(r * 255.0f);
        auto finalG = static_cast<uint8_t>(g * 255.0f);
        auto finalB = static_cast<uint8_t>(b * 255.0f);

        return (0xFF << 24) | (finalR << 16) | (finalG << 8) | finalB;
    }

    inline uint32_t DefaultPixelShader(const FragmentInput& in) {
        return PackColor(in.vertexColor);
    }

    inline uint32_t LambertPixelShader(const FragmentInput& in) {
        const Vector3 lightPos{ 5.0f, 5.0f, -5.0f };
        const float ambient = 0.2f;

        Vector3 N = in.normal.Normalized();
        Vector3 L = (lightPos - in.posWorld).Normalized();

        float dotNL = N.Dot(L);
        float diffuseIntensity = std::max(0.0f, dotNL);

        VertexColor outColor;
        outColor.r = in.vertexColor.r * diffuseIntensity + ambient;
        outColor.g = in.vertexColor.g * diffuseIntensity + ambient;
        outColor.b = in.vertexColor.b * diffuseIntensity + ambient;

        return PackColor(outColor);
    }
}
