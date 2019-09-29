#ifndef CHIP_8_EMULATOR_DISPLAY_H
#define CHIP_8_EMULATOR_DISPLAY_H

#include <iostream>
#include <SDL2/SDL.h>
#include <array>
#include "Chip8.h"

class Chip8;

const int WIDTH = 1024;
const int HEIGHT = 512;

class Display {
private:
    void sdlInit();
    void rendererInit();
    void textureInit();

    void setKeysDown(Chip8* cpu, SDL_Event event);
    void setKeysUp(Chip8* cpu, SDL_Event event);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
public:
    Display() {
        sdlInit();
        rendererInit();
        textureInit();
    }
    void drawFrame(std::array<uint32_t, DISPLAY_SIZE> pixels);
    void handleEvents(Chip8* cpu);
};

#endif
