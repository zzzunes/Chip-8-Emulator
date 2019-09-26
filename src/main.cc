#include <iostream>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
#include "chip_8.h"

const int WIDTH = 1280;
const int HEIGHT = 720;

bool validArgs(int argc) {
	if (argc not_eq 2) {
		std::cout << "Usage: chip-8 <ROM>" << std::endl;
		return false;
	}
	return true;
}

static SDL_Window* sdlInit() {
	SDL_Window* window = nullptr;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "Error: Could not initialize SDL drawing." << std::endl;
		exit(EXIT_FAILURE);
	}

	window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

	if (not window) {
		std::cout << "Could not create window. \n" << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	return window;
}

static SDL_Renderer* rendererInit(SDL_Window* window) {
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
	return renderer;
}

static SDL_Texture* textureInit(SDL_Renderer* renderer) {
	return SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
}

static void setKeysDown(chip_8& cpu, SDL_Event event) {
	for (int i = 0; i < 16; ++i) {
		if (event.key.keysym.sym == cpu.keymap[i]) {
			cpu.keypad[i] = 1;
		}
	}
}

static void setKeysUp(chip_8& cpu, SDL_Event event) {
	for (int i = 0; i < 16; ++i) {
		if (event.key.keysym.sym == cpu.keymap[i]) {
			cpu.keypad[i] = 0;
		}
	}
}

int main(int argc, char* argv[]) {
	if (not validArgs(argc)) exit(EXIT_FAILURE);
    chip_8 cpu = chip_8();
	SDL_Window* window = sdlInit();
    SDL_Renderer* renderer = rendererInit(window);
    SDL_Texture* texture = textureInit(renderer);
    if (not cpu.load(argv[1])) return -1;
	std::array<uint32_t, DISPLAY_SIZE> pixels {};

    game_loop:
        cpu.execute_instruction();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return -1;

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) return -1;
                if (event.key.keysym.sym == SDLK_RCTRL) asm("int3");
                setKeysDown(cpu, event);
            }

            if (event.type == SDL_KEYUP) {
                setKeysUp(cpu, event);
            }
        }

    if (cpu.needs_to_draw) {
        cpu.needs_to_draw = false;

        for (int i = 0; i < 2048; i++) {
            unsigned char pixel = cpu.screen[i];
            unsigned int color = 0x00FF7F00;
            pixels[i] = color * pixel | 0xFF000000;
        }

        SDL_UpdateTexture(texture, nullptr, pixels.begin(), 64 * sizeof(unsigned int));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    std::this_thread::sleep_for(std::chrono::microseconds(1400));
    goto game_loop;
}
