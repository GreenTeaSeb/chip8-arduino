#include <Arduino.h>
#include <Keypad.h>
#include <stdint.h>
#include "chip8.hpp"
#include "keypad.hpp"
#include "display.hpp"
#include "sd.hpp"

Keypad pad = Keypad( makeKeymap(keypad::keys), keypad::row_pins, keypad::col_pins, 4,4 );
String page_roms[5] = {};
uint8_t current_page = {};
uint8_t current_rom = {};
bool started =  false;

void launch_game(){
    if(chip8::drawFlag){
        display::draw(chip8::pixels);
        chip8::drawFlag = false;
    }
    chip8::cycle();
    if (pad.getKeys()){
        for (uint8_t i = 0; i < 16; i++){
          if ( pad.key[i].stateChanged )
          {
            switch (pad.key[i].kstate){
              case PRESSED:{
                chip8::set_key(pad.key[i].kchar,1);
                break;
               }
              case RELEASED:{
                 chip8::set_key(pad.key[i].kchar,0);
                break;
                }
            }
          }
        }
     }
}

void main_menu(){
if (pad.getKeys()){
    for (uint8_t i = 0; i < 16; i++){
      if (pad.key[i].stateChanged ){
         if (pad.key[i].kstate == PRESSED){
             switch(pad.key[i].kchar){
                 case 0x6:{
                     if (!SD::get_roms(++current_page, page_roms) )
                         current_page--;
                     break;
                 }
                 case 0x4:{
                     if(current_page)
                        current_page--;
                     SD::get_roms(current_page, page_roms);
                     break;
                 }
                 case 0x2:{
                    current_rom--;
                    current_rom %= 5;
                    break;
                 }
                 case 0x8:{
                    current_rom++;
                    current_rom %= 5;
                    break;
                 }
                case 0xC:{
                    chip8::setup();
                    if(!SD::load_rom(page_roms[current_rom], chip8::memory)){
                       display::message("failed to open rom");
                       delay(1000);
                       break;
                    }
                    started=true;
                 }
             }
             display::page(page_roms,current_rom);
         }
      }
    }
  }
}

void setup(void) {
   display::setup();
   if (!SD::setup()){
       while(1){
        display::message("Failed to open SD");
       }
   }
   SD::get_roms(current_page, page_roms);
   display::page(page_roms,current_rom);
}

void loop(void) {
    if(!started)
        main_menu();
    else
        launch_game();
}

