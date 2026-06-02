#include "device_executor.h"
#include "../api/mosaic_buffers.h"
#include <cstring>
#include <iostream>

static uint8_t ColorSat(float num) {
    if (num < 0.0f) return 0;
    if (num > 255.0f) return 255;
    return static_cast<uint8_t>(num);
}

static float Edge(float ax, float ay,
                  float bx, float by,
                  float px, float py) {
    return (px - ax) * (by - ay)
         - (py - ay) * (bx - ax);
}

static void RasterizeTriangle(const Vertex& v0,
                              const Vertex& v1,
                              const Vertex& v2,
                              uint32_t* vram,
                              int width,
                              int x0, int y0,
                              int x1, int y1) {
    float area = Edge(v0.position.x, v0.position.y,
                      v1.position.x, v1.position.y,
                      v2.position.x, v2.position.y);

    if (area == 0) return;

    Vertex a = v0;
    Vertex b = v1;
    Vertex c = v2;

    if (area < 0) {
        area = -area;
        std::swap(b, c);
    }

    // "Unpacking" colors of each vertex
    uint32_t colorA = a.color;
    float aR = static_cast<float>((colorA >> 16) & 0xFF);
    float aG = static_cast<float>((colorA >> 8)  & 0xFF);
    float aB = static_cast<float>((colorA)       & 0xFF);

    uint32_t colorB = b.color;
    float bR = static_cast<float>((colorB >> 16) & 0xFF);
    float bG = static_cast<float>((colorB >> 8)  & 0xFF);
    float bB = static_cast<float>((colorB)       & 0xFF);

    uint32_t colorC = c.color;
    float cR = static_cast<float>((colorC >> 16) & 0xFF);
    float cG = static_cast<float>((colorC >> 8)  & 0xFF);
    float cB = static_cast<float>((colorC)       & 0xFF);

    const float invArea = 1.0f / area;

    float aR_inv = aR * invArea; float aG_inv = aG * invArea; float aB_inv = aB * invArea;
    float bR_inv = bR * invArea; float bG_inv = bG * invArea; float bB_inv = bB * invArea;
    float cR_inv = cR * invArea; float cG_inv = cG * invArea; float cB_inv = cB * invArea;

    for (int y = y0; y < y1; ++y) {
        for (int x = x0; x < x1; ++x) {
            float px = x + 0.5f;
            float py = y + 0.5f;

            float w0 = Edge(b.position.x, b.position.y,
                            c.position.x, c.position.y,
                            px, py);

            float w1 = Edge(c.position.x, c.position.y,
                            a.position.x, a.position.y,
                            px, py);

            float w2 = Edge(a.position.x, a.position.y,
                            b.position.x, b.position.y,
                            px, py);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float r = w0 * aR_inv + w1 * bR_inv + w2 * cR_inv;
                float g = w0 * aG_inv + w1 * bG_inv + w2 * cG_inv;
                float b = w0 * aB_inv + w1 * bB_inv + w2 * cB_inv;

                uint8_t finalR = ColorSat(r);
                uint8_t finalG = ColorSat(g);
                uint8_t finalB = ColorSat(b);

                vram[y * width + x] = (0xFF << 24) | (finalR << 16) | (finalG << 8) | finalB;
            }
        }
    }
}

void MosaicDeviceExecutor::BinTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, int width, int height) {
    float minX = std::min({ v0.position.x, v1.position.x, v2.position.x });
    float maxX = std::max({ v0.position.x, v1.position.x, v2.position.x });
    float minY = std::min({ v0.position.y, v1.position.y, v2.position.y });
    float maxY = std::max({ v0.position.y, v1.position.y, v2.position.y });

    int minPixelX = static_cast<int>(std::floor(minX));
    int maxPixelX = static_cast<int>(std::ceil(maxX)) - 1;
    int minPixelY = static_cast<int>(std::floor(minY));
    int maxPixelY = static_cast<int>(std::ceil(maxY)) - 1;

    minPixelX = std::max(0, minPixelX);
    minPixelY = std::max(0, minPixelY);
    maxPixelX = std::min(width - 1, maxPixelX);
    maxPixelY = std::min(height - 1, maxPixelY);

    int startTileX = std::max(0, minPixelX / TILE_SIZE);
    int endTileX   = std::min(m_tileGrid.tileCountX - 1, maxPixelX / TILE_SIZE);
    int startTileY = std::max(0, minPixelY / TILE_SIZE);
    int endTileY   = std::min(m_tileGrid.tileCountY - 1, maxPixelY / TILE_SIZE);

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
                    const Vertex& v0 = vertices[indices[i]];
                    const Vertex& v1 = vertices[indices[i + 1]];
                    const Vertex& v2 = vertices[indices[i + 2]];

                    BinTriangle(v0, v1, v2, width, height);
                }

                // RENDER
                for (const auto& tile : m_tileGrid.tiles) {
                    if (tile.primitives.empty())
                        continue;

                    int pixelStartX = tile.x * TILE_SIZE;
                    int pixelStartY = tile.y * TILE_SIZE;

                    int pixelEndX = std::min(width,  pixelStartX + TILE_SIZE);
                    int pixelEndY = std::min(height, pixelStartY + TILE_SIZE);

                    // raster bounds per tile
                    for (const auto& prim : tile.primitives) {
                        RasterizeTriangle(
                            prim.v0,
                            prim.v1,
                            prim.v2,
                            vram,
                            width,
                            pixelStartX,
                            pixelStartY,
                            pixelEndX,
                            pixelEndY
                        );
                    }
                }
                break;
            }
        }
    }
}
