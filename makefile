# --- mega2560 ide 1.6
BOARD_TAG    = mega
BOARD_SUB    = atmega2560
ARDUINO_DIR  = /opt/arduino

ARDUINO_LIBS = SPI Wire U8g2 Keypad SdFat
CFLAGS_STD = -std=gnu11 -std=gnu11
CXXFLAGS_STD = -std=gnu++17
include /usr/share/arduino/Arduino.mk
