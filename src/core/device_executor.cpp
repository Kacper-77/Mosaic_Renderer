#include "device_executor.h"
#include "../api/mosaic_buffers.h"
#include <cstring>
#include <iostream>

void MosaicDeviceExecutor::BinTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
    float minX = std::min({v0.position.x, v1.position.x, v2.position.x});
    float maxX = std::max({v0.position.x, v1.position.x, v2.position.x});
    float minY = std::min({v0.position.y, v1.position.y, v2.position.y});
    float maxY = std::max({v0.position.y, v1.position.y, v2.position.y});

    int startTileX = std::max(0, static_cast<int>(minX) / TILE_SIZE);
    int endTileX   = std::min(m_tileGrid.tileCountX - 1, static_cast<int>(maxX) / TILE_SIZE);
    int startTileY = std::max(0, static_cast<int>(minY) / TILE_SIZE);
    int endTileY   = std::min(m_tileGrid.tileCountY - 1, static_cast<int>(maxY) / TILE_SIZE);

    RasterPrimitive prim = { v0, v1, v2 };
    for (int ty = startTileY; ty <= endTileY; ++ty) {
        for (int tx = startTileX; tx <= endTileX; ++tx) {
            int tileIndex = ty * m_tileGrid.tileCountX + tx;
            m_tileGrid.tiles[tileIndex].primitives.push_back(prim);
        }
    }
}

void MosaicDeviceExecutor::Execute(const MosaicCommandBuffer& cmdBuffer, uint32_t* vram, int width, int height) {
    if (!m_gridInitialized) {
        m_tileGrid.Initialize(width, height);
        m_gridInitialized = true;
    }

    const uint8_t* data = cmdBuffer.GetData();
    size_t size = cmdBuffer.GetSize();
    size_t pc = 0;

    while (pc < size) {
        CommandType type = static_cast<CommandType>(data[pc]);
        pc += 1;

        switch (type) {
            case CommandType::Clear: {
                CommandClear cmd;
                std::memcpy(&cmd, &data[pc], sizeof(CommandClear));
                pc += sizeof(CommandClear);

                for (int i = 0; i < width * height; ++i) vram[i] = cmd.color;
                m_tileGrid.ClearPrimitives();
                break;
            }
            case CommandType::BindVertexBuffer: {
                CommandBindVertexBuffer cmd;
                std::memcpy(&cmd, &data[pc], sizeof(CommandBindVertexBuffer));
                pc += sizeof(CommandBindVertexBuffer);
                m_currentVertexBuffer = cmd.buffer;
                break;
            }
            case CommandType::BindIndexBuffer: {
                CommandBindIndexBuffer cmd;
                std::memcpy(&cmd, &data[pc], sizeof(CommandBindIndexBuffer));
                pc += sizeof(CommandBindIndexBuffer);
                m_currentIndexBuffer = cmd.buffer;
                break;
            }
            case CommandType::DrawIndexed: {
                CommandDrawIndexed cmd;
                std::memcpy(&cmd, &data[pc], sizeof(CommandDrawIndexed));
                pc += sizeof(CommandDrawIndexed);

                if (!m_currentVertexBuffer || !m_currentIndexBuffer) break;

                const Vertex* vertices = m_currentVertexBuffer->GetRawData();
                const uint32_t* indices = m_currentIndexBuffer->GetRawData();

                // BINNING PASS
                for (uint32_t i = 0; i < cmd.indexCount; i += 3) {
                    const Vertex& v0 = vertices[indices[i + 0]];
                    const Vertex& v1 = vertices[indices[i + 1]];
                    const Vertex& v2 = vertices[indices[i + 2]];

                    BinTriangle(v0, v1, v2);
                }

                // RENDER / DEBUG PASS
                for (const auto& tile : m_tileGrid.tiles) {
                    if (tile.primitives.empty()) continue;

                    int pixelStartX = tile.x * TILE_SIZE;
                    int pixelEndX   = std::min(width, pixelStartX + TILE_SIZE);
                    int pixelStartY = tile.y * TILE_SIZE;
                    int pixelEndY   = std::min(height, pixelStartY + TILE_SIZE);

                    for (int y = pixelStartY; y < pixelEndY; ++y) {
                        for (int x = pixelStartX; x < pixelEndX; ++x) {
                            if (x == pixelStartX || x == pixelEndX - 1 || y == pixelStartY || y == pixelEndY - 1) {
                                vram[y * width + x] = 0xFF00FF00; // Zielony kolor diagnostyczny
                            }
                        }
                    }
                }
                break;
            }
        }
    }
}
