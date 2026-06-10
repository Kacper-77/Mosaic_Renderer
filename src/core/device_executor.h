#pragma once

#include "../api/command_buffer.h"
#include "mosaic_tiling.h"
#include <cstdint>

class MosaicDeviceExecutor {
private:
    MosaicTileGrid m_tileGrid;
    bool m_gridInitialized = false;
    // Pipeline State Registers
    const MosaicVertexBuffer* m_currentVertexBuffer = nullptr;
    const MosaicIndexBuffer*  m_currentIndexBuffer  = nullptr;

    std::vector<float> m_zBuffer;
    std::vector<uint32_t> vram;

public:
    void Execute(const MosaicCommandBuffer& cmdBuffer, uint32_t* vram, int width, int height);
    void BinTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, int width, int height);

    void SetVram(const long long int x, uint32_t val) {
        vram = std::vector(x, val);
    }

    uint32_t* GetVram() {
        return vram.data();
    }

};
