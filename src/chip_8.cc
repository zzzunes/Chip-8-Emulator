#include <iostream>
#include <fstream>
#include "chip_8.h"

void chip_8::init(void) {
    instruction_pointer = 200;
    opcode = 0;
    index_regiser = 0;
    stack_pointer = 0;

    for (int i = 0; i < 16; i++) {
        V[i] = 0;
        stack[i] = 0;
        keypad[i] = 0;
    }

    for (unsigned char x : screen) x = 0;
    for (unsigned char x : memory) x = 0;

    // Load fonts into lowest memory (0-80)
    for (int i = 0; i < FONTSET_SIZE; i++) {
        memory[i] = fontset[i];
    }

    delay_timer = 0;
    sound_timer = 0;
}

bool chip_8::load(std::string file_path) {
    init();
    // Read rom into memory
}

bool chip_8::execute_instruction() {
    // Gets the opcode and handles it appropriately
    // Manages timers
}
