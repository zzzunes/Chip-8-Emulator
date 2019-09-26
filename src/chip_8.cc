#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include "chip_8.h"

void chip_8::init(void) {
    instruction_pointer = 0x200;
    opcode = 0;
    index_register = 0;
    stack_pointer = 0;

    for (int i = 0; i < 16; i++) {
        V[i] = 0;
        stack[i] = 0;
        keypad[i] = 0;
    }

    for (unsigned char x : screen) x = 0;
    for (unsigned char x : memory) x = 0;

    for (int i = 0; i < FONTSET_SIZE; i++) {
        memory[i] = fontset[i];
    }

    delay_timer = 0;
    sound_timer = 0;
}

bool chip_8::load(std::string file_path) {
    init();

    std::cout << "Loading ROM..." << std::endl;
    std::ifstream input_rom;

    input_rom.open(file_path, std::ios::in);
    if (not input_rom.is_open()) {
        std::cout << "Failed to load ROM." << std::endl;
        return false;
    }

    // Get file size by offset and return to beginning for reading
    input_rom.seekg(0, input_rom.end);
    long rom_size = input_rom.tellg();
    input_rom.seekg(0, input_rom.beg);

    char* buffer = new char[rom_size];
    input_rom.read(buffer, rom_size);

    if (input_rom) {
        std::cout << "ROM read successfully." << std::endl;
    }

    // Enough space?
    if ((MEM_SIZE - LOWER_MEMORY) > rom_size) {
        for (int i = 0; i < rom_size; i++) {
            memory[LOWER_MEMORY + i] = (unsigned char) buffer[i];
        }
    }
    else {
        std::cout << "Not enough space in memory available for this ROM." << std::endl;
        delete[] buffer;
        input_rom.close();
        return false;
    }

    delete[] buffer;
    input_rom.close();
    return true;
}

bool chip_8::execute_instruction() {
    // opcode is two bytes: one higher byte for current location and one lower byte combine for one opcode
    opcode = memory[instruction_pointer] << 8 | memory[instruction_pointer + 1];
    bool opcode_success = true;

    switch (opcode & 0xF000) {
        case 0x0000: {
            switch (opcode & 0x000F) {
                // 00E0 - Clear
                case 0x0000: {
                    for (unsigned char x : screen) x = 0;
                    needs_to_draw = true;
                    instruction_pointer += 2;
                    break;
                }
                    // 00EE - Return from subroutine
                case 0x000E: {
                    instruction_pointer = stack[--stack_pointer];
                    instruction_pointer += 2;
                    break;
                }

                default: {
                    std::cout << "ROM includes unknown opcode. 1" << std::endl;
                    opcode_success = false;
                }
            }
            break;
        }
        // 1NNN - Jumps to NNN
        case 0x1000: {
            instruction_pointer = opcode & 0xFFF;
            break;
        }

        // 2NNN - Calls subroutine at NNN
        case 0x2000: {
            stack[stack_pointer++] = instruction_pointer;
            instruction_pointer = opcode & 0x0FFF;
            break;
        }

        // 3[X]NN - Skips next instruction if register V[X] == NN
        // The term V[opcode & 0x0F00] >> 8 finds V[X] since the registers are 16 bits
        case 0x3000: {
            if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                instruction_pointer += 4;
            }
            else instruction_pointer += 2;
            break;
        }

        // 4[X]NN - Skips next instruction if register V[X] != NN
        case 0x4000: {
            if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                instruction_pointer += 4;
            }
            else instruction_pointer += 2;
            break;
        }

        // 5XY0 - Skips next instruction if V[X] == VY
        case 0x5000: {
            if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
                instruction_pointer += 4;
            }
            else instruction_pointer += 2;
            break;
        }

        // 6XNN - Sets V[X] to NN
        case 0x6000: {
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            instruction_pointer += 2;
            break;
        }

        // 7XNN - adds NN to V[X]
        case 0x7000: {
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            instruction_pointer += 2;
            break;
        }

        case 0x8000: {
            switch (opcode & 0x000F) {
                // 8XY0 - Sets V[X] to V[Y]
                case 0x0000: {
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    instruction_pointer += 2;
                    break;
                }

                // 8XY1 - Sets V[X] to V[X] || V[Y]
                case 0x0001: {
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    instruction_pointer += 2;
                    break;
                }

                // 8XY2 - Sets V[X] to V[X] & V[Y]
                case 0x0002: {
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    instruction_pointer += 2;
                    break;
                }

                // 8XY3 - Sets V[X] to V[X] ^ V[Y]
                case 0x0003: {
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    instruction_pointer += 2;
                    break;
                }

                // 8XY4 - Adds V[Y] to V[X], VF is set to 1 if there's carry
                case 0x0004: {
                    if (V[(opcode & 0x0F00) >> 8] <= V[(opcode & 0x00F0) >> 4]) V[0xF] = 1;
                    else V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    instruction_pointer += 2;
                    break;
                }

                // 8XY5 - Subtracts V[Y] from V[X], VF is set to 0 if there's borrow
                case 0x0005: {
                    if (V[(opcode & 0x0F00) >> 8] <= V[(opcode & 0x00F0) >> 4]) V[0xF] = 0;
                    else V[0xF] = 1;
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    instruction_pointer += 2;
                    break;
                }

                // 8XY6 - Stores least significant bit of V[X] in VF then V[X] >> 1
                case 0x0006: {
                    V[0xF] = V[opcode & 0x0F00 >> 8] & 0x1;
                    V[(opcode & 0x0F00) >> 8] >>= 1;
                    instruction_pointer += 2;
                    break;
                }

                // 8XY7 - Sets V[X] to V[Y] minus V[X], VF is set to 0 if there's borrow
                case 0x0007: {
                    if (V[(opcode & 0x00F0) >> 4] <= V[(opcode & 0x0F00) >> 8]) V[0xF] = 0;
                    else V[0xF] = 1;
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    instruction_pointer += 2;
                    break;
                }

                // 8XYE - Stores most significant bit of V[X] in V[F] then V[X] << 1
                case 0x000E: {
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0xF;
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    instruction_pointer += 2;
                    break;
                }

                default: {
                    std::cout << "ROM includes unknown opcode. 2" << std::endl;
                    opcode_success = false;
                }
            }
            break;
        }

        // 9XY0 - Skips next instruction if V[X] != V[Y]
        case 0x9000: {
            if (V[opcode & 0x0F00 >> 8] != V[opcode & 0x00F0 >> 4]) instruction_pointer += 4;
            else instruction_pointer += 2;
            break;
        }

        // ANNN - Sets Index Register to NNN
        case 0xA000: {
            index_register = opcode & 0x0FFF;
            instruction_pointer += 2;
            break;
        }

        // BNNN - Sets instruction pointer to NNN plus V[0]
        case 0xB000: {
            instruction_pointer = (opcode & 0x0FFF) + V[0];
            break;
        }

        // CXNN - Sets V[X] to result of a random number bitwise and with NN
        case 0xC000: {
            // Random generation from user Cornstalks on StackOverflow:
            // https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
            std::mt19937 rng;
            rng.seed(std::random_device()());
            std::uniform_int_distribution<std::mt19937::result_type> random_range(0,255);

            V[opcode & 0x0F00 >> 8] = random_range(rng) & (opcode & 0x00FF);
            instruction_pointer += 2;
            break;
        }

        // DXYN - Draws a sprite at coordinate (V[X], V[Y]) that has width of 8 pixels
        // and height N pixels. Each row is read as bit-coded starting from Index Register.
        // VF is set to 1 if any pixels are flipped from set --> unset; VF set to 0 otherwise
        case 0xD000: {
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
                pixel = memory[index_register + yline];
                for(int xline = 0; xline < 8; xline++)
                {
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(screen[(x + xline + ((y + yline) * 64))] == 1)
                        {
                            V[0xF] = 1;
                        }
                        screen[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }

            needs_to_draw = true;
            instruction_pointer += 2;
            break;
        }

        // Key operations
        case 0xE000: {
            switch (opcode & 0x000F) {
                // EX9E - Skips next instruction if key stored in V[X] is pressed
                case 0x000E: {
                    if (keypad[V[(opcode & 0x0F00) >> 8]]) instruction_pointer +=4;
                    else instruction_pointer += 2;
                    break;
                }

                // EXA1 - Skips next instruction if key stored in V[X} ISN'T pressed
                case 0x0001: {
                    if (not keypad[V[(opcode & 0x0F00) >> 8]]) instruction_pointer +=4;
                    else instruction_pointer += 2;
                    break;
                }

                default: {
                    std::cout << "ROM includes unknown opcode. 3" << std::endl;
                    opcode_success = false;
                }
            }
            break;
        }

        case 0xF000: {
            switch (opcode & 0x00FF) {
                // FX07 - Sets V[X] to value of delay timer
                case 0x0007: {
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    instruction_pointer += 2;
                    break;
                }

                // FX0A - Awaits key press, stores key in V[X]
                case 0x000A: {
                    bool obtained_key = false;

                    for (int i = 0; i < 16; i++) {
                        if (keypad[i]) {
                            V[(opcode & 0x0F00) >> 8] = i;
                            obtained_key = true;
                        }
                    }

                    // No key pressed, leave like normal but don't advance instruction pointer so we try again
                    if (not obtained_key) break;

                    instruction_pointer += 2;
                    break;
                }

                // FX15 - Sets delay timer to V[X]
                case 0x0015: {
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    instruction_pointer += 2;
                    break;
                }

                // FX18 - Sets sound timer to V[X]
                case 0x0018: {
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    instruction_pointer += 2;
                    break;
                }

                // FX1E - Sets index register to V[X]
                case 0x001E: {
                    if(index_register + V[(opcode & 0x0F00) >> 8] > 0xFFF) V[0xF] = 1;
                    else V[0xF] = 0;

                    index_register += V[(opcode & 0x0F00) >> 8];
                    instruction_pointer += 2;
                    break;
                }

                // FX29 - Sets index register to location of sprite for the character in V[X]
                // Remember, the fontset array in chip_8.h works in sets of five
                case 0x0029: {
                    index_register = V[(opcode & 0x0F00) >> 8] * 0x5;
                    instruction_pointer += 2;
                    break;
                }

                // FX33 - Stores the binary coded decimal representation of V[X]
                // at addresses of index, index + 1, index + 2
                // Three most significant, middle digit, least significant
                // For BCD info --> https://www.electronics-tutorials.ws/binary/binary-coded-decimal.html
                case 0x0033: {
                    memory[index_register] = V[(opcode & 0x0F00) >> 8] / 100; // Three most
                    memory[index_register+1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10; // Middle digit
                    memory[index_register+2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10; // Least significant

                    instruction_pointer += 2;
                    break;
                }

                // Almost there..
                // FX55 - Stores V[0] to V[X] in memory starting at index. The offset from index is
                // increased by 1 for each value written
                case 0x0055: {
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
                        memory[index_register + i] = V[i];
                    }

                    index_register += ((opcode & 0x0F00) >> 8) + 1;
                    instruction_pointer += 2;
                    break;
                }

                // Finally!
                // FX65 - Fills V[0] to V[X] with values from memory starting at address of the index register.
                // Again, offset from index is increased by 1 for each value written
                case 0x0065: {
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
                        V[i] = memory[index_register + i];
                    }

                    index_register += ((opcode & 0x0F00) >> 8) + 1;
                    instruction_pointer += 2;
                    break;
                }

                default: {
                    std::cout << "ROM includes unknown opcode. 4" << std::endl;
                    opcode_success = false;
                }
            }
            break;
        }

        default: {
            std::cout << "ROM includes unknown opcode: 5" << std::endl;
            opcode_success = false;
        }
    }

    if (delay_timer > 0) delay_timer--;
    if (sound_timer > 0) {
        if (sound_timer == 1);
        sound_timer--;
    }

    return opcode_success;
}
