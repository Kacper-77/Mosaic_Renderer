#pragma once

#include "mosaic_types.h" 
#include <algorithm>
#include <cmath>

struct FragmentInput {
    float x, y, z;          
    float wInv;       
    Vector3 posWorld;      
    Vector3 normal;   
    uint32_t vertexColor;
};

namespace Shaders {

    inline uint32_t DefaultPixelShader(const FragmentInput& in) {
        return in.vertexColor;
    }

    inline uint32_t LambertPixelShader(const FragmentInput& in) {
        const Vector3 lightPos{ 5.0f, 5.0f, -5.0f };
        const uint32_t ambientColor = 0xFF1A1A1A;

        Vector3 N = in.normal.Normalized();

        Vector3 L = (lightPos - in.posWorld).Normalized();

        float dotNL = N.Dot(L);
        
        float diffuseIntensity = std::max(0.0f, dotNL);

        uint8_t r = (in.vertexColor >> 16) & 0xFF;
        uint8_t g = (in.vertexColor >> 8)  & 0xFF;
        uint8_t b = in.vertexColor & 0xFF;

        int finalR = std::min(255, static_cast<int>(r * diffuseIntensity) + 0x1A);
        int finalG = std::min(255, static_cast<int>(g * diffuseIntensity) + 0x1A);
        int finalB = std::min(255, static_cast<int>(b * diffuseIntensity) + 0x1A);

        return (0xFF << 24) | (finalR << 16) | (finalG << 8) | finalB;
    }
}
