#include <iostream>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
#include "chip_8.h"
#include "display.h"

using namespace Display;

int main(int argc, char* argv[]) {
    if (not argc == 2) {
        std::cout << "Usage: chip_8 <ROM>" << std::endl;
        exit(EXIT_FAILURE);
    }
    chip_8 cpu = chip_8();
    SDL_Window* window = Display::sdlInit();
    SDL_Renderer* renderer = Display::rendererInit(window);
    SDL_Texture* texture = Display::textureInit(renderer);
    std::array<uint32_t, DISPLAY_SIZE> pixels;
    if (not cpu.load(argv[1])) exit(EXIT_FAILURE);

    game_loop:
        cpu.cycle();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) exit(EXIT_SUCCESS);
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) exit(EXIT_SUCCESS);
                if (event.key.keysym.sym == SDLK_RCTRL) asm("int3");
                Display::setKeysDown(cpu, event);
            }
            if (event.type == SDL_KEYUP) {
                Display::setKeysUp(cpu, event);
            }
        }

        if (cpu.needs_to_draw) {
            cpu.needs_to_draw = false;
            cpu.loadVideoRamInto(pixels);
            Display::drawFrame(renderer, texture, pixels);
        }

        std::this_thread::sleep_for(std::chrono::microseconds(1400));
        goto game_loop;
}
