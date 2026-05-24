#include <SDL2/SDL.h>
#include <iostream>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Initialization Failed! Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Mosaic-Engine | Sanity Check",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window Creation Failed! Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    std::cout << "Initialized successfully. Window open." << std::endl;

    bool is_running = true;
    SDL_Event event;

    while (is_running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                is_running = false;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "Clean shutdown complete." << std::endl;
    return 0;
}
