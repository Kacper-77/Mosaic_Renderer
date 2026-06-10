#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "api/command_buffer.h"
#include "api/mosaic_buffers.h"
#include "core/device_executor.h"

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    SDL_Window* window = SDL_CreateWindow("### Mosaic Engine ###", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* streaming_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    Vertex quadVertices[] = {
        { { -0.5f,  0.5f, 0.0f, 1.0f }, 0xFF00FF00 }, 
        { {  0.5f,  0.5f, 0.0f, 1.0f }, 0xFF00FF00 }, 
        { {  0.5f, -0.5f, 0.0f, 1.0f }, 0xFF0000FF }, 
        { { -0.5f, -0.5f, 0.0f, 1.0f }, 0xFF000000 }  
    };

    uint32_t quadIndices[] = {
        0, 1, 2,
        0, 2, 3
    };

    Vertex cubeVertices[] = {
        { { -0.5f,  0.5f,  0.5f, 1.0f }, 0xFFFF0000 }, 
        { {  0.5f,  0.5f,  0.5f, 1.0f }, 0xFF00FF00 }, 
        { {  0.5f, -0.5f,  0.5f, 1.0f }, 0xFF0000FF }, 
        { { -0.5f, -0.5f,  0.5f, 1.0f }, 0xFFFFFF00 }, 

        { { -0.5f,  0.5f, -0.5f, 1.0f }, 0xFFFF00FF }, 
        { {  0.5f,  0.5f, -0.5f, 1.0f }, 0xFF00FFFF }, 
        { {  0.5f, -0.5f, -0.5f, 1.0f }, 0xFFFFFFFF }, 
        { { -0.5f, -0.5f, -0.5f, 1.0f }, 0xFF808080 }  
    };

    uint32_t cubeIndices[] = {
        0, 1, 2,  0, 2, 3,
        4, 6, 5,  4, 7, 6,
        4, 1, 0,  4, 5, 1,
        3, 2, 6,  3, 6, 7,
        4, 0, 3,  4, 3, 7,
        1, 5, 6,  1, 6, 2
    };

    MosaicVertexBuffer vbo;
    vbo.SetData(cubeVertices, 8);

    MosaicIndexBuffer ibo;
    ibo.SetData(cubeIndices, 36);

    MosaicCommandBuffer cmdBuffer;
    MosaicDeviceExecutor executor;

    executor.SetVram(SCREEN_WIDTH * SCREEN_HEIGHT, 0xFF000000);

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
        cmdBuffer.CmdClear(0xFAFAFA);
        
        cmdBuffer.CmdBindVertexBuffer(&vbo);
        cmdBuffer.CmdBindIndexBuffer(&ibo);
        
        cmdBuffer.CmdDrawIndexed(36);

        executor.Execute(cmdBuffer, executor.GetVram(), SCREEN_WIDTH, SCREEN_HEIGHT);

        SDL_UpdateTexture(streaming_texture, nullptr, executor.GetVram(), SCREEN_WIDTH * sizeof(uint32_t));
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
