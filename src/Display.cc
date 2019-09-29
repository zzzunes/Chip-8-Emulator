#include "Display.h"
#include "Chip8.h"

void Display::sdlInit() {
    SDL_Window* window = nullptr;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "Error: Could not initialize SDL drawing." << std::endl;
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("Emerald Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    if (not window) {
        std::cout << "Could not create window. \n" << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    this->window = window;
}

void Display::rendererInit() {
    SDL_Renderer *renderer = SDL_CreateRenderer(this->window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
    this->renderer = renderer;
}

void Display::textureInit() {
    this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
}

void Display::setKeysDown(Chip8& cpu, SDL_Event event) {
    for (int i = 0; i < 16; ++i) {
        if (event.key.keysym.sym == cpu.keymap[i]) {
            cpu.keypad[i] = 1;
        }
    }
}

void Display::setKeysUp(Chip8& cpu, SDL_Event event) {
    for (int i = 0; i < 16; ++i) {
        if (event.key.keysym.sym == cpu.keymap[i]) {
            cpu.keypad[i] = 0;
        }
    }
}

void Display::drawFrame(std::array<uint32_t, DISPLAY_SIZE> pixels) {
    SDL_UpdateTexture(texture, nullptr, pixels.begin(), 64 * sizeof(unsigned int));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Display::handleEvents(Chip8& cpu) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) exit(EXIT_SUCCESS);
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) exit(EXIT_SUCCESS);
            if (event.key.keysym.sym == SDLK_RCTRL) asm("int3");
            setKeysDown(cpu, event);
        }
        if (event.type == SDL_KEYUP) {
            setKeysUp(cpu, event);
        }
    }
}
