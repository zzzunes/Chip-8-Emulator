#include <iostream>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
#include "chip_8.h"
#include "display.h"

using namespace Display;

bool validArgs(int argc) {
	if (argc not_eq 2) {
		std::cout << "Usage: chip-8 <ROM>" << std::endl;
		return false;
	}
	return true;
}

int main(int argc, char* argv[]) {
	if (not validArgs(argc)) exit(EXIT_FAILURE);
    chip_8 cpu = chip_8();
	SDL_Window* window = Display::sdlInit();
    SDL_Renderer* renderer = Display::rendererInit(window);
    SDL_Texture* texture = Display::textureInit(renderer);
    if (not cpu.load(argv[1])) return -1;
	std::array<uint32_t, DISPLAY_SIZE> pixels;

    game_loop:
        cpu.cycle();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return -1;

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) return -1;
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
        SDL_UpdateTexture(texture, nullptr, pixels.begin(), 64 * sizeof(unsigned int));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    std::this_thread::sleep_for(std::chrono::microseconds(1400));
    goto game_loop;
}
