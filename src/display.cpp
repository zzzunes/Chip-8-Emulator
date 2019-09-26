#include <iostream>
#include "display.h"
#include "chip_8.h"

namespace Display {
	SDL_Window* sdlInit() {
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

	SDL_Renderer* rendererInit(SDL_Window* window) {
		SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
		SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
		return renderer;
	}

	SDL_Texture* textureInit(SDL_Renderer* renderer) {
		return SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
	}

	void setKeysDown(chip_8& cpu, SDL_Event event) {
		for (int i = 0; i < 16; ++i) {
			if (event.key.keysym.sym == cpu.keymap[i]) {
				cpu.keypad[i] = 1;
			}
		}
	}

	void setKeysUp(chip_8& cpu, SDL_Event event) {
		for (int i = 0; i < 16; ++i) {
			if (event.key.keysym.sym == cpu.keymap[i]) {
				cpu.keypad[i] = 0;
			}
		}
	}
};

