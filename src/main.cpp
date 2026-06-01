#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "api/command_buffer.h"
#include "api/mosaic_buffers.h"
#include "core/device_executor.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    SDL_Window* window = SDL_CreateWindow("### Mosaic Engine ###", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* streaming_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    std::vector<uint32_t> virtual_vram(SCREEN_WIDTH * SCREEN_HEIGHT, 0xFF000000);

    Vertex quadVertices[] = {
        { { 540.0f, 260.0f, 0.0f, 1.0f }, 0xFF00FF00 }, 
        { { 740.0f, 260.0f, 0.0f, 1.0f }, 0xFF00FF00 }, 
        { { 740.0f, 460.0f, 0.0f, 1.0f }, 0xFF0000FF }, 
        { { 600.0f, 600.0f, 0.0f, 1.0f }, 0xFF000000 },
        { { 100.0f, 250.0f, 0.0f, 1.0f }, 0xEE4B2B }, 
        { { 200.0f, 200.0f, 0.0f, 1.0f }, 0xEE4B2B },
        { { 200.0f, 360.0f, 0.0f, 1.0f }, 0xFF00FF00 }
    };

    uint32_t quadIndices[] = {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6 
    };

    MosaicVertexBuffer vbo;
    vbo.SetData(quadVertices, 7);

    MosaicIndexBuffer ibo;
    ibo.SetData(quadIndices, 9);

    MosaicCommandBuffer cmdBuffer;
    MosaicDeviceExecutor executor;

    bool is_running = true;
    SDL_Event event;
    uint8_t color_cycle = 0;

    while (is_running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) is_running = false;
        }

        color_cycle++;
        uint32_t clear_color = (0xFF << 24) | (color_cycle << 16) | (20 << 8) | 30;

        cmdBuffer.Reset();
        cmdBuffer.CmdClear(0xFAFAFAFA);
        
        cmdBuffer.CmdBindVertexBuffer(&vbo);
        cmdBuffer.CmdBindIndexBuffer(&ibo);
        
        cmdBuffer.CmdDrawIndexed(9);

        executor.Execute(cmdBuffer, virtual_vram.data(), SCREEN_WIDTH, SCREEN_HEIGHT);

        SDL_UpdateTexture(streaming_texture, nullptr, virtual_vram.data(), SCREEN_WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, streaming_texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyTexture(streaming_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
