#include "sd.hpp"

SdFat sd;
SdFile dir;

bool SD::setup()
{
      pinMode(53,OUTPUT);
      if (!sd.begin(53, SPI_HALF_SPEED))
         return false;
      dir.open("/");
      return true;
}

bool  SD::get_roms(uint8_t page, String arr[]) {
    dir.rewind();
    SdFile file;
    for(uint8_t i = 0 ; i < page*5 ; ++i){
        if (! file.openNext(&dir,O_READ))
          return false;
    }
    for(uint8_t i = 0 ; i < 5 ; ++i){
      file.openNext(&dir,O_READ);
      char name[32];
      file.getName(name,32);
      arr[i] = name;
    }
    return true;

}


bool SD::load_rom(String name, uint8_t mem[])
{
    if(!dir.open(name.c_str()))
        return false;
    uint16_t index = 0x200;
     if (!dir) {
       return false;
     }
     Serial.println(name);
     while (dir.available()) {
       mem[index++] = dir.read();
     }

     dir.close();

     return true;
}


