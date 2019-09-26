#ifndef CHIP_8_CHIP_8_H
#define CHIP_8_CHIP_8_H

#include <array>
#include <SDL2/SDL.h>

const int DISPLAY_SIZE = 2048;
const int MEM_SIZE = 4096;
const int FONTSET_SIZE = 80;
const int LOWER_MEMORY = 512; // Reserved for opcodes, fonts

class chip_8 {
private:
    std::array<unsigned char, MEM_SIZE> memory;
    std::array<unsigned char, 16> V; // Registers V0-VF

    std::array<unsigned short, 16> stack;
    unsigned short stack_pointer;

    unsigned short instruction_pointer;
    unsigned short index_register;
    unsigned short opcode;

    unsigned char delay_timer;

    void init(void);
	void fetch_opcode(void);
	void execute_instruction(void);

public:
	bool load(std::string file_path);
	void cycle(void);
	void loadVideoRamInto(std::array<uint32_t, DISPLAY_SIZE>& pixels);

    std::array<unsigned char, 16> keypad;
    std::array<unsigned char, DISPLAY_SIZE> screen;
    bool needs_to_draw;

    const std::array<unsigned char, FONTSET_SIZE> fontset {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80, // F
    };

    // For SDL obtaining keypress, updating keypad
    const std::array<unsigned char, 16> keymap {
            SDLK_x, SDLK_1, SDLK_2, SDLK_3, SDLK_q,
            SDLK_w, SDLK_e, SDLK_a, SDLK_s, SDLK_d,
            SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_f, SDLK_v,
    };
};

#endif
