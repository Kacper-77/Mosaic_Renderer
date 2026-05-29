#pragma once

#include <vector>
#include <cstdint>
#include "../api/mosaic_types.h"

constexpr int TILE_SIZE = 64;

struct RasterPrimitive {
    Vertex v0, v1, v2;
};

struct MosaicTile {
    int x, y;
    std::vector<RasterPrimitive> primitives;
};

class MosaicTileGrid {
public:
    int tileCountX;
    int tileCountY;
    std::vector<MosaicTile> tiles;

    void Initialize(int screenWidth, int screenHeight) {
        tileCountX = (screenWidth + TILE_SIZE - 1) / TILE_SIZE;
        tileCountY = (screenHeight + TILE_SIZE - 1) / TILE_SIZE;
        
        tiles.resize(tileCountX * tileCountY);

        for (int y = 0; y < tileCountY; ++y) {
            for (int x = 0; x < tileCountX; ++x) {
                int index = y * tileCountX + x;
                tiles[index].x = x;
                tiles[index].y = y;
                tiles[index].primitives.clear();
            }
        }
    }

    void ClearPrimitives() {
        for (auto& tile : tiles) {
            tile.primitives.clear();
        }
    }
};
