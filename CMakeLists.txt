cmake_minimum_required(VERSION 3.14)
project(Chip-8-Emulator)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -lSDL2)
add_executable(Chip8 src/main.cc src/chip_8.h src/chip_8.cc src/display.cpp src/display.h)