#ifndef HD44102_h
#define HD44102_h

#include <Arduino.h>

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
    void begin(byte _cs1pin, byte _cs2pin, byte _rwpin, byte _enablepin,
               byte _dipin, bool _8bitbus, byte d0, byte d1, byte d2,
               byte d3, byte d4, byte d5, byte d6, byte d7);
    void writedata(byte value, byte chip);
    byte readdata(byte chip);
    void displayon(byte chip);
    void displayoff(byte chip);
    void modeup(byte chip);
    void modedown(byte chip);
    void startpage(byte page, byte chip);
    void setxy(byte x, byte y, byte chip);

  private:
    volatile uint8_t *_en_port_pin;
    uint8_t _en_bit_mask;
    volatile uint8_t *_data_port_pin[12], *_read_data_port_pin[8];
    uint8_t _data_bit_mask[12];
    void pulseEnable(void);
    void pulseEnable5(void);
    void dataPinWrite(uint8_t pin, bool value);
    bool dataPinRead(uint8_t pin);
    void write8bits(byte value);
    byte readFlags(void);
    bool readBusy(void);
    void sendcmd(byte value, byte chip);

};
#endif
