#include <Arduino.h>
#include <string.h>
#include "chip8.hpp"


static uint16_t opcode = {};
static uint8_t V[16] = {};
static uint16_t indx = {};
static uint16_t pc = {};
uint8_t chip8::memory[] = {};
uint8_t chip8::pixels[] = {};

static uint8_t delayT = {};
static uint8_t soundT = {};
static uint16_t stack[16] = {};
static uint16_t stackPointer = {};
static uint8_t key[16] = {};
static uint16_t cylceCounter = {};

uint8_t chip8::drawFlag = {};
static uint8_t keyPressed = {};
static uint8_t awaitKey = {};
static uint8_t keyAwaited = {};


void chip8::setup()
{
    pinMode(9, OUTPUT); // beep
    pc = 0x200;
    opcode = 0;
    indx = 0;
    stackPointer = 0;
    drawFlag = false;
    for (uint8_t i = 0; i < 80; i++) {
      memory[i] = pgm_read_byte(chip8_fontset + i);
    }
}


void chip8::awaitKeyPress(uint16_t X)
{
    awaitKey = true;
    if (keyPressed) {
      V[X] = keyAwaited;
      pc += 2;
      keyPressed = false;
      awaitKey = false;
    }
}

 void chip8::skipNextInst()
{
    pc += 4;
}

bool chip8::get_pixel(uint8_t row, uint8_t col){
    return ( pixels[( row* WIDTH  + col )/ 8 ] >> (col % 8)) & 1;
};

bool chip8::set_pixel(uint8_t row, uint8_t col, bool pixel){
    if(pixel)
        pixels[(row * WIDTH + col )/ 8] |= (1 << (col % 8));
    else
       pixels[(row * WIDTH + col )/ 8]  &= ~(1 << (col % 8));
};

void chip8::draw(uint8_t x, uint8_t y, uint8_t h)
{
    V[0xF] = 0;
    for (uint8_t i = 0; i < h; i++) {
      for (uint8_t j = 0; j < SPRITE_SIZE; j++) {
        bool curPix = get_pixel( (y+ i) % HEIGHT, (x + j) % WIDTH );
        bool nexPix = memory[(indx + i) % MEMORY_SIZE] & (0x80 >> j);
        if (nexPix != 0) {
          if (curPix == 1) {
            V[0xF] = 1;
          }
          set_pixel((y+ i) % HEIGHT, (x + j) % WIDTH,  curPix ^ nexPix );
        }
      }
    }
    chip8::drawFlag = true;
}



void chip8::cycle()
{
    pc %= MEMORY_SIZE;
    stackPointer %= 16;
    indx %= MEMORY_SIZE;
    cylceCounter %= SLEEP;
    // get opcode
    opcode = memory[pc] << 8 | memory[(pc + 1) % MEMORY_SIZE];
    uint16_t Y = (opcode & 0x00F0) >> 4;
    uint16_t X = (opcode & 0x0F00) >> 8; // 0XY0;

    // decode
    switch (opcode & 0xF000) {
      // do op codes that are dependant on 1st bit
      case 0x1000: // 1NNN
        pc = opcode & 0x0FFF;
        break;
      case 0x2000: // 2NNN
        pc += 2;
        ++stackPointer;
        stackPointer %= 16;
        stack[stackPointer] = pc;
        pc = opcode & 0x0FFF;
        break;
      case 0x3000: // 3XNN
        if (V[X] == (opcode & 0x00FF)) {
          skipNextInst();
        } else {
          pc += 2;
        }
        break;
      case 0x4000: // 4XNN
        if (V[X] != (opcode & 0x00FF)) {
          skipNextInst();
        } else {
          pc += 2;
        }
        break;
      case 0x5000: // 5XY0
        if (V[X] == V[Y]) {
          skipNextInst();
        } else {
          pc += 2;
        }
        break;
      case 0x6000: // 6XNN
        V[X] = opcode & 0x00FF;
        pc += 2;
        break;
      case 0x7000: // 7XNN
        V[X] += opcode & 0x0FF;
        pc += 2;
        break;
      case 0x9000: // 9XY0
        if (V[X] != V[Y]) {
          skipNextInst();
        } else {
          pc += 2;
        }
        break;
      case 0xA000: // ANNN
        indx = opcode & 0x0FFF;
        pc += 2;
        break;

      case 0xB000: // BNNN
        pc = (opcode & 0x0FFF) + V[0];
        break;
      case 0xC000: // CXNN
        V[X] = (random(256)) & (opcode & 0x00FF);
        pc += 2;
        break;
      case 0xD000: // DXYN DRAWING
        draw(V[X], V[Y], opcode & 0x000F);
        pc += 2;
        break;

        // op codes that are dependant on other bits
      case 0x0000:
        switch (opcode & 0x000F) {
          case 0x0000: // 0x00E0
            memset(pixels,0,sizeof(pixels));
            chip8::drawFlag  = true;
            pc += 2;
            break;
          case 0x000E: // 0x0EE
            pc = stack[stackPointer];
            --stackPointer;
            break;
        }
        break;

      case 0x8000:
        switch (opcode & 0x000F) {
          case 0x0000: // 8XY0
            V[X] = V[Y];
            pc += 2;
            break;
          case 0x0001: // 8XY1
            V[X] = V[X] | V[Y];
            pc += 2;
            break;
          case 0x0002: // 8XY2
            V[X] = V[X] & V[Y];
            pc += 2;
            break;
          case 0x0003: // 8XY3
            V[X] = V[X] ^ V[Y];
            pc += 2;
            break;
          case 0x0004: // 8XY4
            V[0xF] = (V[X] + V[Y] > 255)
                       ? 1
                       : 0; // if sum is greater than 255 then there is carry
            V[X] += V[Y];
            pc += 2;
            break;
          case 0x0005: // 8XY5
            V[0xF] = V[X] > V[Y];
            V[X] -= V[Y];
            pc += 2;
            break;
          case 0x0006: // 8XY6
            V[0xF] = V[X] & 1;
            V[X] >>= 1;
            pc += 2;
            break;
          case 0x0007:                      // 8XY7
            V[0xF] = (V[Y] > V[X]) ? 1 : 0; // check for borrow
            V[X] = V[Y] - V[X];
            pc += 2;
            break;
          case 0x000E: // 8XYE
            V[0xF] = (V[X] >> 7) & 1;
            V[X] <<= 1;
            pc += 2;
            break;
        }
        break;

      case 0xE000:
        switch (opcode & 0x00FF) {
          case 0x009E: // EX9E
            if (key[V[X] % 16] != 0) {
              skipNextInst();
            } else {
              pc += 2;
            }
            break;
          case 0x00A1: // EXA1
            if (key[V[X] % 16] == 0) {
              skipNextInst();
            } else {
              pc += 2;
            }
            break;
        }
        break;

      case 0xF000:
        switch (opcode & 0x00FF) {
          case 0x0007: // FX07
            V[X] = delayT;
            pc += 2;
            break;
          case 0x000A: // FX0A await keypress
            awaitKeyPress(X);
            break;
          case 0x0015: // FX15
            delayT = V[X];
            pc += 2;
            break;
          case 0x0018: // FX18
            soundT = V[X];
            pc += 2;
            break;
          case 0x001E: // FX1E
            indx += V[X];
            pc += 2;
            break;
          case 0x0029: // FX29
            indx = V[X] * 5;
            pc += 2;
            break;
          case 0x0033: // FX33 BCD
            memory[indx] = V[X] / 100;
            memory[(indx + 1) % MEMORY_SIZE] = (V[X] / 10) % 10;
            memory[(indx + 2) % MEMORY_SIZE] = V[X] % 10;
            pc += 2;
            break;
          case 0x0055: // FX55
            for (int i = 0; i <= X; i++) {

              memory[(indx + i) % MEMORY_SIZE] = V[i];
            }
            pc += 2;
            break;
          case 0x0065: // FX65
            for (int i = 0; i <= X; i++) {
              V[i] = memory[(indx + i) % MEMORY_SIZE];
            }
            pc += 2;
            break;
        }

        break;
    }

    // timers
    if (++cylceCounter == SLEEP) {
      if (delayT > 0)
        --delayT;
      if (soundT > 0) {
          analogWrite(9, 20);
        --soundT;
      }else{
          analogWrite(9, 0);
      }
    }
}



void chip8::set_key(uint8_t k, bool state)
{
    k = k == 0x1f ? 0x0 : k;
    key[k] = state;
    keyAwaited = k;
    keyPressed = state;
}
