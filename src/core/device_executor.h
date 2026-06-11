#pragma once

#include "../api/command_buffer.h"
#include "mosaic_tiling.h"
#include <cstdint>
#include <vector>
#include <algorithm>

class MosaicDeviceExecutor {
private:
    MosaicTileGrid m_tileGrid;
    bool m_gridInitialized = false;
    
    // Pipeline State Registers
    const MosaicVertexBuffer* m_currentVertexBuffer = nullptr;
    const MosaicIndexBuffer* m_currentIndexBuffer  = nullptr;

    // Buffers
    std::vector<float>    m_zBuffer;
    std::vector<uint32_t> m_vram;
    
    int m_width = 0;
    int m_height = 0;

public:
    explicit MosaicDeviceExecutor(int width, int height) {
        ResizeBuffers(width, height);
    }

    void Execute(const MosaicCommandBuffer& cmdBuffer);
    void BinTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, int width, int height);
    void RasterizeTriangle(const Vertex& v0,
                              const Vertex& v1,
                              const Vertex& v2,
                              int x0, int y0,
                              int x1, int y1);

    uint32_t* GetVram() { return m_vram.data(); }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    
    // SETTER / RESIZER
    void ResizeBuffers(int width, int height) { 
        m_width = width;
        m_height = height;
        m_vram.resize(static_cast<size_t>(width * height)); 
        m_zBuffer.resize(static_cast<size_t>(width * height));
    }
    
    void ClearBuffers(uint32_t clearColor) { 
        std::fill(m_vram.begin(), m_vram.end(), clearColor); 
        std::fill(m_zBuffer.begin(), m_zBuffer.end(), 1.0f); 
    }
};
