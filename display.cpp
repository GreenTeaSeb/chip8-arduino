#include "display.hpp"
#include <Arduino.h>


static U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 12, /* CS=*/ 11, /* reset=*/ 8);
void display::setup()
{
     u8g2.begin();
}

void display::draw(uint8_t pixels[])
{
    u8g2.firstPage();
     do {
       for(uint8_t row = 0; row < 32 ; ++row){
           for(uint8_t bit = 0; bit < 64 ; ++bit){
               if( (pixels[(row * 64 + bit) / 8 ] >> (bit % 8)) & 1){
                   u8g2.setDrawColor(2);
//                   u8g2.drawPixel(bit, row);
                   u8g2.drawPixel(bit*2     , row*2    );
                   u8g2.drawPixel(bit*2 + 1 , row*2    );
                   u8g2.drawPixel(bit*2  ,    row*2 + 1);
                   u8g2.drawPixel(bit*2 + 1 , row*2 + 1);
                }
           }
       }
//       u8g2.setDrawColor(1);
//       u8g2.setBitmapMode(0);
//       u8g2.drawXBM(32,16,64,32,pixels);
     } while ( u8g2.nextPage()
    );
}

void display::message(String msg)
{
    u8g2.firstPage();
     do {
       u8g2.setFont(u8g2_font_t0_11_tf);
       u8g2.drawStr(0,8,msg.c_str());
     } while ( u8g2.nextPage() );
}


void display::page(String roms[],uint8_t selected)
{
    u8g2.clear();
    u8g2.firstPage();
     do {
       u8g2.setFont(u8g2_font_t0_11_tf);
       for(uint8_t i = 1; i <= 5; ++i){
           if(i == selected+1){
               String name = "*";
               name.concat(roms[i-1]);
               u8g2.drawStr(0,9*i, name.c_str());
           }else
           u8g2.drawStr(0,9*i, roms[i-1].c_str());
       }
     } while ( u8g2.nextPage() );
}
