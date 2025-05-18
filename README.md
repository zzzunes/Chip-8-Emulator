Chip-8 emulator written in C++  with the SDL multimedia library.

# What is Chip-8?

Chip-8 is an interpreted programming language used on computers such as the COSMAC VIP in the 70's. Chip-8 programs run on a virtual machine. Many games in the public domain were written for the Chip-8 such as pong, pac-man, and tetris.

# Build Instructions
First, clone the repository and build the project:

``` 
$ git clone https://github.com/zzzunes/Chip-8-Emulator/
$ cd Chip-8-Emulator/src
$ mkdir build; cd build
$ cmake ..
$ make
```    
Then, to run the emulator:

``` 
$ ./Chip8 ../Roms/PONG
```


# Screenshots

![Alt text](/Screenshots/brix.gif?raw=true "Pong!")

# Additional Information

Opcodes and Chip-8 specifics obtained from these sources:

http://mattmik.com/files/chip8/mastering/chip8.html

https://en.wikipedia.org/wiki/CHIP-8
