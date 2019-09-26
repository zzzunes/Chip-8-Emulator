#ifndef CHIP_8_EMULATOR_DISPLAY_H
#define CHIP_8_EMULATOR_DISPLAY_H

#include <SDL2/SDL.h>
#include "chip_8.h"

const int WIDTH = 1280;
const int HEIGHT = 720;

namespace Display {
    SDL_Window* sdlInit();
    SDL_Renderer* rendererInit(SDL_Window* window);
    SDL_Texture* textureInit(SDL_Renderer* renderer);
    void setKeysDown(chip_8& cpu, SDL_Event event);
    void setKeysUp(chip_8& cpu, SDL_Event event);
}

#endif
