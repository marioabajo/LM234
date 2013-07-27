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

void HD44102::pulseEnable(void)
{
  digitalWrite(en, LOW);
  digitalWrite(en, HIGH);
  digitalWrite(en, LOW);
}

void HD44102::write8bits(byte value)
/* Write data to the lcd in 8bit bus mode */
{
  byte i;
  
  if (rwstatus == 0)
  {
    for (i=0;i<8;i++)
      pinMode(datos[i],OUTPUT);
    rwstatus=1;
  }
  digitalWrite(datos[0],(value & 0x01));
  digitalWrite(datos[1],(value & 0x02));
  digitalWrite(datos[2],(value & 0x04));
  digitalWrite(datos[3],(value & 0x08));
  digitalWrite(datos[4],(value & 0x10));
  digitalWrite(datos[5],(value & 0x20));
  digitalWrite(datos[6],(value & 0x40));
  digitalWrite(datos[7],(value & 0x80));
  
  pulseEnable();
}

byte HD44102::readFlags(void)
/* Read the lcd processor status flags: BUSY, up/down, on/off and reset */
{
  bool busy_flag;
  bool updown_flag;
  bool onoff_flag;
  bool reset_flag;

  digitalWrite(di,LOW);
  digitalWrite(rw,HIGH);

  pulseEnable();

  if (rwstatus == 1)
  {
    pinMode(datos[7],INPUT);
    pinMode(datos[6],INPUT);
    pinMode(datos[5],INPUT);
    pinMode(datos[4],INPUT);
    rwstatus=0;
  }

  busy_flag   = digitalRead(datos[7]);
  updown_flag = digitalRead(datos[6]);
  onoff_flag  = digitalRead(datos[5]);
  reset_flag  = digitalRead(datos[4]);

  return busy_flag + (updown_flag << 1) + (onoff_flag << 2) + (reset_flag << 3);
}

bool HD44102::readBusy(void)
/* Read busy flag */
{
  return readFlags() & 0x01;
}

void HD44102::sendcmd(byte value, byte chip)
/* Send a command with params to the lcd processor */
{
  while (HD44102::readBusy());

  digitalWrite(cs1, chip & 0x01);
  digitalWrite(cs2, chip & 0x02);
  digitalWrite(di,LOW);
  digitalWrite(rw,LOW);
  
  write8bits(value);
}

void HD44102::writedata(byte value, byte chip)
{
  digitalWrite(cs1, chip & 0x01);
  digitalWrite(cs2, chip & 0x02);
  digitalWrite(di,HIGH);
  digitalWrite(rw,LOW);
  
  write8bits(value);
}

byte HD44102::readdata(byte chip)
{
  byte i,j=0;

  digitalWrite(cs1, chip & 0x01);
  digitalWrite(cs2, chip & 0x02);
  digitalWrite(di,HIGH);
  digitalWrite(rw,HIGH);
  pulseEnable();
  
  if (rwstatus == 1)
  {
    for (i=0;i<8;i++)
      pinMode(datos[i],INPUT);
    rwstatus=0;
  }
  
  j += digitalRead(datos[0]);
  j += digitalRead(datos[1]) << 1;
  j += digitalRead(datos[2]) << 2;
  j += digitalRead(datos[3]) << 3;
  j += digitalRead(datos[4]) << 4;
  j += digitalRead(datos[5]) << 5;
  j += digitalRead(datos[6]) << 6;
  j += digitalRead(datos[7]) << 7;
  
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

