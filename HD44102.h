#ifndef HD44102_h
#define HD44102_h

#include "Arduino.h"

#define byte uint8_t

class HD44102 {
  public:
    HD44102(void);
    byte datos[8];
    byte cs1;  // Chip select 1
    byte cs2;  // Chip select 2
    byte en;   // Enable pin
    byte di;   // Data / instruction pin
    byte rw;   // Read / Write Pin
    bool _8bits, rwstatus;
    void writedata(byte value, byte chip);
    byte readdata(byte chip);
    void displayon(byte chip);
    void displayoff(byte chip);
    void modeup(byte chip);
    void modedown(byte chip);
    void startpage(byte page, byte chip);
    void setxy(byte x, byte y, byte chip);

  private:
    void pulseEnable(void);
    void write8bits(byte value);
    byte readFlags(void);
    bool readBusy(void);
    void sendcmd(byte value, byte chip);

};
#endif
