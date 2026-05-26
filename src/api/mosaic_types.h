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
