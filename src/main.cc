#include <iostream>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
#include "Chip8.h"
#include "Display.h"

int main(int argc, char* argv[]) {
    if (not argc == 2) {
        std::cout << "Usage: Chip8 <ROM>" << std::endl;
        exit(EXIT_FAILURE);
    }
    Chip8* cpu = new Chip8();
    Display* display = new Display();
    std::array<uint32_t, DISPLAY_SIZE> pixels {};
    if (not cpu->load(argv[1])) exit(EXIT_FAILURE);

    game_loop:
        cpu->cycle();
        cpu->draw(display, pixels);
        display->handleEvents(cpu);

        std::this_thread::sleep_for(std::chrono::microseconds(1500));
        goto game_loop;
}
