cmake_minimum_required(VERSION 3.14)
project(Chip-8-Emulator)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_LIBRARIES -lSDL2)
add_executable(Chip8 src/main.cc src/Chip8.h src/Chip8.cc src/Display.cc src/Display.h)