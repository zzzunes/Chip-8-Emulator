#ifndef CHIP_8_EMULATOR_DISPLAY_H
#define CHIP_8_EMULATOR_DISPLAY_H

#include <iostream>
#include <SDL2/SDL.h>
#include "chip_8.h"

const int WIDTH = 1024;
const int HEIGHT = 512;

class Display {
private:
    void sdlInit();
    void rendererInit();
    void textureInit();

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
public:
    Display() {
        sdlInit();
        rendererInit();
        textureInit();
    }
    void setKeysDown(chip_8& cpu, SDL_Event event);
    void setKeysUp(chip_8& cpu, SDL_Event event);
    void drawFrame(std::array<uint32_t, DISPLAY_SIZE> pixels);
};

#endif
