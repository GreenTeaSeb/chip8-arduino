#pragma once
#include <SD.h>
namespace sd {
    bool setup();
    bool load_rom(String name, uint8_t mem[]);
    void get_roms(uint8_t page, String arr[]);
} // namespace sd
