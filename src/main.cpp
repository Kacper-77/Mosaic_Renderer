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

    // ### CONE ###
    const int CONE_SEGMENTS = 32;
    const float CONE_RADIUS = 0.5f;
    const float CONE_HEIGHT = 1.0f;

    std::vector<Vertex> coneVertices;
    std::vector<uint32_t> coneIndices;

    coneVertices.push_back({ { 0.0f, CONE_HEIGHT * 0.5f, 0.0f, 1.0f }, 0xFFFF0000 });
    coneVertices.push_back({ { 0.0f, -CONE_HEIGHT * 0.5f, 0.0f, 1.0f }, 0xFF0000FF });

    for (int i = 0; i < CONE_SEGMENTS; ++i) {
        float angle = (static_cast<float>(i) / CONE_SEGMENTS) * 2.0f * M_PI;
        float x = CONE_RADIUS * std::cos(angle);
        float z = CONE_RADIUS * std::sin(angle);
        float y = -CONE_HEIGHT * 0.5f;

        uint8_t r = static_cast<uint8_t>((x + CONE_RADIUS) / (2.0f * CONE_RADIUS) * 255);
        uint8_t g = static_cast<uint8_t>((z + CONE_RADIUS) / (2.0f * CONE_RADIUS) * 255);
        uint32_t color = (0xFF << 24) | (r << 16) | (g << 8) | 0xFF;

        coneVertices.push_back({ { x, y, z, 1.0f }, color });
    }

    for (int i = 0; i < CONE_SEGMENTS; ++i) {
        uint32_t current = 2 + i;
        uint32_t next = 2 + (i + 1) % CONE_SEGMENTS;

        coneIndices.push_back(0);
        coneIndices.push_back(current);
        coneIndices.push_back(next);
        coneIndices.push_back(1);
        coneIndices.push_back(next);
        coneIndices.push_back(current);
    }

    // ### SPHERE ###
    const int SPHERE_X_SEGMENTS = 64; 
    const int SPHERE_Y_SEGMENTS = 64;
    const float SPHERE_RADIUS = 0.9f;

    std::vector<Vertex> sphereVertices;
    std::vector<uint32_t> sphereIndices;

    for (int y = 0; y <= SPHERE_Y_SEGMENTS; ++y) {
        float ySegment = static_cast<float>(y) / SPHERE_Y_SEGMENTS;
        float angleY = ySegment * M_PI;

        for (int x = 0; x <= SPHERE_X_SEGMENTS; ++x) {
            float xSegment = static_cast<float>(x) / SPHERE_X_SEGMENTS;
            float angleX = xSegment * 2.0f * M_PI;

            float xPos = SPHERE_RADIUS * std::sin(angleY) * std::cos(angleX);
            float yPos = SPHERE_RADIUS * std::cos(angleY);
            float zPos = SPHERE_RADIUS * std::sin(angleY) * std::sin(angleX);

            uint8_t r = static_cast<uint8_t>((xPos / SPHERE_RADIUS * 0.5f + 0.5f) * 255);
            uint8_t g = static_cast<uint8_t>((yPos / SPHERE_RADIUS * 0.5f + 0.5f) * 255);
            uint8_t b = static_cast<uint8_t>((zPos / SPHERE_RADIUS * 0.5f + 0.5f) * 255);
            uint32_t color = (0xFF << 24) | (r << 16) | (g << 8) | b;

            sphereVertices.push_back({ { xPos, yPos, zPos, 1.0f }, color });
        }
    }


    for (int y = 0; y < SPHERE_Y_SEGMENTS; ++y) {
        for (int x = 0; x < SPHERE_X_SEGMENTS; ++x) {
            uint32_t first = (y * (SPHERE_X_SEGMENTS + 1)) + x;
            uint32_t second = first + SPHERE_X_SEGMENTS + 1;

            sphereIndices.push_back(first);
            sphereIndices.push_back(second);
            sphereIndices.push_back(first + 1);
            sphereIndices.push_back(second);
            sphereIndices.push_back(second + 1);
            sphereIndices.push_back(first + 1);
        }
    }

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
    MosaicDeviceExecutor executor(1080, 720);

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

        executor.Execute(cmdBuffer);

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
