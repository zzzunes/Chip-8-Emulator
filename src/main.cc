#include <iostream>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
#include "chip_8.h"
#include "display.h"

int main(int argc, char* argv[]) {
    if (not argc == 2) {
        std::cout << "Usage: chip_8 <ROM>" << std::endl;
        exit(EXIT_FAILURE);
    }
    chip_8 cpu = chip_8();
    Display* display = new Display();
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
                display->setKeysDown(cpu, event);
            }
            if (event.type == SDL_KEYUP) {
                display->setKeysUp(cpu, event);
            }
        }

        if (cpu.needs_to_draw) {
            cpu.needs_to_draw = false;
            cpu.loadVideoRamInto(pixels);
            display->drawFrame(pixels);
        }

        std::this_thread::sleep_for(std::chrono::microseconds(1400));
        goto game_loop;
}
