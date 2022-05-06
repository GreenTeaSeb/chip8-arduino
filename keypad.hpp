#pragma once
#include <stdint.h>
#include <stdlib.h>

namespace keypad {
    constexpr uint8_t row_pins[] =  {31,33,35,37};
    constexpr uint8_t col_pins[] = {23,25,27,29};
    constexpr char keys[4][4] = {
    {0x1,0x2,0x3, 0xC},
    {0x4,0x5,0x6, 0xD},
    {0x7,0x8,0x9, 0xE},
    {0xA,0x1F,0xB, 0xF}
    };
}