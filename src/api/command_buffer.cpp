#include "command_buffer.h"
#include <cstring>

void MosaicCommandBuffer::Reset() {
    m_buffer.clear();
}

void MosaicCommandBuffer::CmdClear(uint32_t color) {
    m_buffer.push_back(static_cast<uint8_t>(CommandType::Clear));
    size_t offset = m_buffer.size();
    m_buffer.resize(offset + sizeof(CommandClear));
    CommandClear cmd = { color };
    std::memcpy(&m_buffer[offset], &cmd, sizeof(CommandClear));
}

void MosaicCommandBuffer::CmdBindVertexBuffer(const MosaicVertexBuffer* vertexBuffer) {
    m_buffer.push_back(static_cast<uint8_t>(CommandType::BindVertexBuffer));
    size_t offset = m_buffer.size();
    m_buffer.resize(offset + sizeof(CommandBindVertexBuffer));
    CommandBindVertexBuffer cmd = { vertexBuffer };
    std::memcpy(&m_buffer[offset], &cmd, sizeof(CommandBindVertexBuffer));
}

void MosaicCommandBuffer::CmdBindIndexBuffer(const MosaicIndexBuffer* indexBuffer) {
    m_buffer.push_back(static_cast<uint8_t>(CommandType::BindIndexBuffer));
    size_t offset = m_buffer.size();
    m_buffer.resize(offset + sizeof(CommandBindIndexBuffer));
    CommandBindIndexBuffer cmd = { indexBuffer };
    std::memcpy(&m_buffer[offset], &cmd, sizeof(CommandBindIndexBuffer));
}

void MosaicCommandBuffer::CmdDrawIndexed(uint32_t indexCount) {
    m_buffer.push_back(static_cast<uint8_t>(CommandType::DrawIndexed));
    size_t offset = m_buffer.size();
    m_buffer.resize(offset + sizeof(CommandDrawIndexed));
    CommandDrawIndexed cmd = { indexCount };
    std::memcpy(&m_buffer[offset], &cmd, sizeof(CommandDrawIndexed));
}
