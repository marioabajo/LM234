#ifndef LcdLM234_h
#define LcdLM234_h

#include "Arduino.h"
#include "HD44102.h"
#include "cp850.h"

#define byte uint8_t

class LcdLM234 : public Print {
  public:
    LcdLM234(const byte _cs1pin, const byte _cs2pin, const byte _enablepin,
             const byte _dipin, const byte _rwpin, const byte d0, const byte d1,
             const byte d2, const byte d3, const byte d4, const byte d5, 
             const byte d6, const byte d7);
    void   lcdon();
    void   lcdoff();
    void   lcdgoto(byte x, byte y);
    void   writedata(byte value);
    byte   read();
    void   cls();
    void   setCursor(byte x, byte y);
    size_t write(byte value);
    void   scrollup(byte x, byte y, byte length, byte height);
    static const byte max_x = 20;
    static const byte max_x_lines = 100;
    static const byte max_y = 8;
    void   invertChr(byte x, byte y);
    void   invertScr(byte x, byte y, byte length, byte height);

    byte global_x;
    byte global_x_lines;
    byte global_y;
    bool autoscroll;
    
  private:
    void calc_pos();
    byte chip;
    bool mode;     // up (1) or down (0)
};

#endif
