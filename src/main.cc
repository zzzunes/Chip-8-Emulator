#include <iostream>
#include <chrono>
#include <thread>

#include "chip_8.h"

const int HEIGHT = 512;
const int WIDTH = 1024;

int main(int argc, char* argv[]) {
    int status;
    
    if (argc not_eq 2) {
        std::cout << "Usage: chip-8 <ROM>" << std::endl;
        return -1;
    }

    chip_8 core = chip_8();
    SDL_Window* window = nullptr;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << "Error: Could not initialize SDL drawing." << std::endl;
        return -1;
    }

    window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                     WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    if (not window) {
        std::cout << "Could not create window. \n" << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

    SDL_Texture* sdl_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                                 SDL_TEXTUREACCESS_STREAMING, 64, 32);

    uint32_t pixels[2048];

    load_rom:
    //if (not core.load(argv[1])) return -1;
    if (not core.load(argv[1])) return -1;

    do {
        status = core.execute_instruction();

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return -1;

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) return -1;

                // Reload ROM
                if (event.key.keysym.sym == SDLK_F1) goto load_rom;

                for (int i = 0; i < 16; ++i) {
                    if (event.key.keysym.sym == core.keymap[i]) {
                        core.keypad[i] = 1;
                    }
                }
            }

            if (event.type == SDL_KEYUP) {
                for (int i = 0; i < 16; ++i) {
                    if (event.key.keysym.sym == core.keymap[i]) {
                        core.keypad[i] = 0;
                    }
                }
            }
        }

    if (core.screen_has_changed) {
        core.screen_has_changed = false;

        for (int i = 0; i < 2048; i++) {
            unsigned char pixel = core.screen[i];
            pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
        }

        SDL_UpdateTexture(sdl_texture, nullptr, pixels, 64 * sizeof(unsigned int));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, sdl_texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    // Without this it goes way too fast
    std::this_thread::sleep_for(std::chrono::microseconds(1200));

    } while (status);

    return 0;
}
