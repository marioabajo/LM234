#include "HD44102.h"
#include <inttypes.h>

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif


HD44102::HD44102(void)
{
}

void HD44102::begin(byte _cs1pin, byte _cs2pin, byte _rwpin, byte _enablepin,
                    byte _dipin, bool _8bitbus, byte d0, byte d1, byte d2, 
                    byte d3, byte d4, byte d5, byte d6, byte d7)
{
  uint8_t i;

  cs1=_cs1pin;
  cs2=_cs2pin;
  rw=_rwpin;
  en=_enablepin;
  di=_dipin;
  datos[0]=d0;
  datos[1]=d1;
  datos[2]=d2;
  datos[3]=d3;
  datos[4]=d4;
  datos[5]=d5;
  datos[6]=d6;
  datos[7]=d7;
  _8bits=_8bitbus;
  rwstatus=false;

  pinMode(cs1,OUTPUT);
  pinMode(cs2,OUTPUT);
  pinMode(di,OUTPUT);
  pinMode(rw,OUTPUT);
  pinMode(en,OUTPUT);

  _en_port_pin=portOutputRegister(digitalPinToPort(en));
  _en_bit_mask=digitalPinToBitMask(en);

  for (i=0;i<8;i++)
  {
    _data_port_pin[i]=portOutputRegister(digitalPinToPort(datos[i]));
    _read_data_port_pin[i]=portInputRegister(digitalPinToPort(datos[i]));
    _data_bit_mask[i]=digitalPinToBitMask(datos[i]);
  }
  _data_port_pin[8]=portOutputRegister(digitalPinToPort(cs1));
  _data_bit_mask[8]=digitalPinToBitMask(cs1);
  _data_port_pin[9]=portOutputRegister(digitalPinToPort(cs2));
  _data_bit_mask[9]=digitalPinToBitMask(cs2);
  _data_port_pin[10]=portOutputRegister(digitalPinToPort(rw));
  _data_bit_mask[10]=digitalPinToBitMask(rw);
  _data_port_pin[11]=portOutputRegister(digitalPinToPort(di));
  _data_bit_mask[11]=digitalPinToBitMask(di);
  
}

inline void HD44102::pulseEnable(void)
{
  //digitalWrite(en, LOW);
  *_en_port_pin &= ~_en_bit_mask;
  //digitalWrite(en, HIGH);
  *_en_port_pin |= _en_bit_mask;
  //digitalWrite(en, LOW);
  *_en_port_pin &= ~_en_bit_mask;
}

inline void HD44102::pulseEnable5(void)
{
  *_en_port_pin &= ~_en_bit_mask;
  delayMicroseconds(5);
  *_en_port_pin |= _en_bit_mask;
  delayMicroseconds(5);
  *_en_port_pin &= ~_en_bit_mask;
}

inline void HD44102::dataPinWrite(uint8_t pin, bool value)
/* Fast digitalWrite */
{
  if (!value)
    *_data_port_pin[pin] &= ~_data_bit_mask[pin];
  else
    *_data_port_pin[pin] |= _data_bit_mask[pin];
}

inline bool HD44102::dataPinRead(uint8_t pin)
/* Fast digitalRead */
{
  return *_read_data_port_pin[pin] & _data_bit_mask[pin];
}

inline void HD44102::write8bits(byte value)
/* Write data to the lcd in 8bit bus mode */
{
  byte i;
  
  if (rwstatus == 0)
  {
    for (i=0;i<8;i++)
      pinMode(datos[i],OUTPUT);
    rwstatus=1;
  }
  dataPinWrite(0,(value & 0x01));
  dataPinWrite(1,(value & 0x02));
  dataPinWrite(2,(value & 0x04));
  dataPinWrite(3,(value & 0x08));
  dataPinWrite(4,(value & 0x10));
  dataPinWrite(5,(value & 0x20));
  dataPinWrite(6,(value & 0x40));
  dataPinWrite(7,(value & 0x80));
  /*digitalWrite(datos[0],(value & 0x01));
  digitalWrite(datos[1],(value & 0x02));
  digitalWrite(datos[2],(value & 0x04));
  digitalWrite(datos[3],(value & 0x08));
  digitalWrite(datos[4],(value & 0x10));
  digitalWrite(datos[5],(value & 0x20));
  digitalWrite(datos[6],(value & 0x40));
  digitalWrite(datos[7],(value & 0x80));*/
  
  pulseEnable5();
}

byte HD44102::readFlags(void)
/* Read the lcd processor status flags: BUSY, up/down, on/off and reset */
{
  bool busy_flag;
  bool updown_flag;
  bool onoff_flag;
  bool reset_flag;

  /*gitalWrite(di,LOW);
  digitalWrite(rw,HIGH);*/
  dataPinWrite(11,LOW);
  dataPinWrite(10,HIGH);

  pulseEnable5();

  if (rwstatus == 1)
  {
    pinMode(datos[7],INPUT);
    pinMode(datos[6],INPUT);
    pinMode(datos[5],INPUT);
    pinMode(datos[4],INPUT);
    rwstatus=0;
  }

  busy_flag   = dataPinRead(7);
  updown_flag = dataPinRead(6);
  onoff_flag  = dataPinRead(5);
  reset_flag  = dataPinRead(4);
  /*busy_flag   = digitalRead(datos[7]);
  updown_flag = digitalRead(datos[6]);
  onoff_flag  = digitalRead(datos[5]);
  reset_flag  = digitalRead(datos[4]);*/

  return busy_flag + (updown_flag << 1) + (onoff_flag << 2) + (reset_flag << 3);
}

inline bool HD44102::readBusy(void)
/* Read busy flag */
{
  return readFlags() & 0x01;
}

inline void HD44102::sendcmd(byte value, byte chip)
/* Send a command with params to the lcd processor */
{
  while (HD44102::readBusy());

  /*digitalWrite(cs1, chip & 0x01);
  digitalWrite(cs2, chip & 0x02);
  digitalWrite(di,LOW);
  digitalWrite(rw,LOW);*/
  dataPinWrite(8,chip & 0x01);
  dataPinWrite(9,chip & 0x02);
  dataPinWrite(11,LOW);
  dataPinWrite(10,LOW);
  
  write8bits(value);
}

void HD44102::writedata(byte value, byte chip)
{
  /*digitalWrite(cs1, chip & 0x01);
  digitalWrite(cs2, chip & 0x02);
  digitalWrite(di,HIGH);
  digitalWrite(rw,LOW);*/
  dataPinWrite(8,chip & 0x01);
  dataPinWrite(9,chip & 0x02);
  dataPinWrite(11,HIGH);
  dataPinWrite(10,LOW);
  
  write8bits(value);
}

byte HD44102::readdata(byte chip)
{
  byte i,j=0;

  /*digitalWrite(cs1, chip & 0x01);
  digitalWrite(cs2, chip & 0x02);
  digitalWrite(di,HIGH);
  digitalWrite(rw,HIGH);*/
  dataPinWrite(8,chip & 0x01);
  dataPinWrite(9,chip & 0x02);
  dataPinWrite(11,HIGH);
  dataPinWrite(10,HIGH);
  pulseEnable5();
  
  if (rwstatus == 1)
  {
    for (i=0;i<8;i++)
      pinMode(datos[i],INPUT);
    rwstatus=0;
  }
  
  j += dataPinRead(0);
  j += dataPinRead(1) << 1;
  j += dataPinRead(2) << 2;
  j += dataPinRead(3) << 3;
  j += dataPinRead(4) << 4;
  j += dataPinRead(5) << 5;
  j += dataPinRead(6) << 6;
  j += dataPinRead(7) << 7;
  /*j += digitalRead(datos[0]);
  j += digitalRead(datos[1]) << 1;
  j += digitalRead(datos[2]) << 2;
  j += digitalRead(datos[3]) << 3;
  j += digitalRead(datos[4]) << 4;
  j += digitalRead(datos[5]) << 5;
  j += digitalRead(datos[6]) << 6;
  j += digitalRead(datos[7]) << 7;*/
  
  return j;
}

void HD44102::displayon(byte chip)
{
  sendcmd(57, chip);
}

void HD44102::displayoff(byte chip)
{
  sendcmd(56, chip);
}

void HD44102::modeup(byte chip)
{
  sendcmd(59, chip);
}

void HD44102::modedown(byte chip)
{
  sendcmd(58, chip);
}

void HD44102::startpage(byte page, byte chip)
{
  if (page<4)
    sendcmd(page + 62, chip);
}

void HD44102::setxy(byte x, byte y, byte chip)
{
  if (y<4 && x<50)
    sendcmd((y << 6) + x, chip);
}

