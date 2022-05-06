#include "sd.hpp"

static File file;

bool sd::setup()
{
      pinMode(53,OUTPUT);
      if (!SD.begin(53))
          return false;
//      file = SD.open("/");
//      file.close();
      file = SD.open("/");
      return true;
}

void  sd::get_roms(uint8_t page, String arr[]) {
  file.rewindDirectory();
  for(uint8_t i = 0 ; i < page*5 ; ++i){
      if (! file.openNextFile()) {
        return;
      }
  }
  for(uint8_t i = 0 ; i < 5 ; ++i){
    File entry =  file.openNextFile();
    if (! entry)
      break;
    arr[i] = entry.name();
    entry.close();
  }
}


bool sd::load_rom(String name, uint8_t mem[])
{
    file.close();
    file = SD.open(name);
    int index = 0x200;
     if (!file) {
       return false;
     }
     Serial.println(name);
     while (file.available()) {
       mem[index++] = file.read();
     }
     file.close();

     return true;
}


