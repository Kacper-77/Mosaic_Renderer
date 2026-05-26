#pragma once
#include <vector>
#include <cstdint>

class MosaicVertexBuffer;
class MosaicIndexBuffer;

enum class CommandType : uint8_t {
    Clear,
    BindVertexBuffer,
    BindIndexBuffer,
    DrawIndexed
};

/* Payloads */
struct CommandClear {
    uint32_t color;
};

struct CommandBindVertexBuffer {
    const MosaicVertexBuffer* buffer;
};

struct CommandBindIndexBuffer {
    const MosaicIndexBuffer* buffer;
};

struct CommandDrawIndexed {
    uint32_t indexCount;
};

class MosaicCommandBuffer {
private:
    std::vector<uint8_t> m_buffer;

public:
    void Reset();
    
    void CmdClear(uint32_t color);
    void CmdBindVertexBuffer(const MosaicVertexBuffer* vertexBuffer);
    void CmdBindIndexBuffer(const MosaicIndexBuffer* indexBuffer);
    void CmdDrawIndexed(uint32_t indexCount);

    const uint8_t* GetData() const { return m_buffer.data(); }
    size_t GetSize() const { return m_buffer.size(); }
};
