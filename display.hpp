#pragma once
#include <U8g2lib.h>
#include <stdint.h>


namespace display {

    void setup();
    void draw(uint8_t pixels[]);
    void message(String msg);
    void page(String roms[], uint8_t selected);

} // namespace display
