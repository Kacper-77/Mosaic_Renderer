#pragma once

#include <vector>
#include "mosaic_types.h"

/* Buffer for Vertex Data */
class MosaicVertexBuffer {
private:
    std::vector<Vertex> m_vertices;

public:
    MosaicVertexBuffer() = default;
    
    // Load to buffer
    void SetData(const Vertex* data, size_t count) {
        m_vertices.assign(data, data + count);
    }

    const Vertex* GetRawData() const { return m_vertices.data(); }
    size_t GetCount() const { return m_vertices.size(); }
};

/* Buffer for Index Data */
class MosaicIndexBuffer {
private:
    std::vector<uint32_t> m_indices;

public:
    MosaicIndexBuffer() = default;

    // Load index data
    void SetData(const uint32_t* data, size_t count) {
        m_indices.assign(data, data + count);
    }

    const uint32_t* GetRawData() const { return m_indices.data(); }
    size_t GetCount() const { return m_indices.size(); }
};
