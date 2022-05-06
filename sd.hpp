#pragma once
#include <SdFat.h>

namespace SD {
    bool setup();
    bool load_rom(String name, uint8_t mem[]);
    bool get_roms(uint8_t page, String arr[]);
} // namespace sd
