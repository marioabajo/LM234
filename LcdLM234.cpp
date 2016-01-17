#include "LcdLM234.h"
#include <avr/pgmspace.h>
#include "HD44102.h"
#include "cp850.h"

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

HD44102 controller;

LcdLM234::LcdLM234(const byte _cs1pin, const byte _cs2pin, const byte _enablepin, 
         const byte _dipin, const byte _rwpin, const byte d0, const byte d1, 
         const byte d2, const byte d3, const byte d4, const byte d5, 
         const byte d6, const byte d7)
{
  controller.begin(_cs1pin, _cs2pin, _rwpin, _enablepin, _dipin, 1, 
                   d0, d1, d2, d3, d4, d5, d6, d7);

  autoscroll=0;
  lcdon();
  cls();
}

void LcdLM234::lcdon()
{
  byte i;
  
  for (i=0;i<4;i++)
  {
    controller.displayon(i);
    controller.startpage(0,i);
    controller.modeup(i);
    controller.setxy(0,0,i);
  }
  global_x=0;
  global_x_lines=0;
  global_y=0;
  chip=0;
  mode=1;
}

void LcdLM234::lcdoff()
{
  byte i;
  
  for (i=0;i<4;i++)
    controller.displayoff(i);
}

void LcdLM234::calc_pos()
{
  byte aux;
  
  // Limits
  if (global_x_lines>=max_x_lines)
  {
    global_x=0;
    global_x_lines=0;
    global_y++;
  }
  if (global_y>=max_y)
  {
    global_y=0;
    global_x_lines=0;
    global_x=0;
  }

  // update controller chip
  aux = (global_x_lines/(max_x_lines/2)) + ((global_y/(max_y/2)) << 1);
  if (aux != chip)
    controller.setxy(global_x_lines%(max_x_lines/2), global_y%(max_y/2), aux);
  chip=aux;
}

void LcdLM234::lcdgoto(byte x, byte y)
{
  byte aux;
  
  global_x = x/5;
  global_x_lines = x;
  global_y = y;
  aux = chip;
  calc_pos();
  if (aux == chip)
    controller.setxy(global_x_lines%(max_x_lines/2), global_y & 0x03, chip);
}

void LcdLM234::writedata(byte value)
{
  controller.writedata(value,chip);
  global_x_lines++;
  global_x=global_x_lines/5;
  calc_pos();
}

byte LcdLM234::read()
{
  byte a;
  
  // Dummy read if the earlier command was "write" and not "read"
  // (datasheet output register section)
  if (controller.rwstatus == true)
    controller.readdata(chip);
  a = controller.readdata(chip);
  global_x_lines++;
  global_x=global_x_lines/5;
  calc_pos();
  return a;
}

void LcdLM234::cls()
{
  byte i,j;
  
  for (j=0;j<max_y;j++)
  {
    lcdgoto(0,j);
    for (i=0;i<max_x_lines;i++)
      writedata(0);
  }
}

void LcdLM234::setCursor(byte x, byte y)
{
  if (x<max_x)
    lcdgoto(x*5,y);
}

size_t LcdLM234::write(byte value)
{
  byte i,j;
  
  for (i=0;i<5;i++)
  {
    j= (byte) pgm_read_byte(charset + value*5 + i);
    writedata(j);
  }
  if (autoscroll && global_y==0 && global_x==0)
  {
    //scrollup(0,0,max_x,max_y-1);
    scrollup();
    setCursor(0,max_y-1);
  }
  return 1;
}

void LcdLM234::scrollup_section(byte x, byte y, byte length, byte height)
{
  byte i,j,k=x+length;
  byte buf[100];

  for (j=y+1;j<=y+height;j++)
  {
    setCursor(x,j);
    for (i=x;i<k*5;i++)
      buf[i] = read();
    setCursor(x,j-1);
    for (i=x;i<k*5;i++)
        writedata(buf[i]);
  }
  setCursor(x,y+height);
  for (i=x;i<k*5;i++)
    writedata(0);
}

void LcdLM234::scrollup()
{
  byte i,j;
  byte buf[4][50];
  byte middle[2][50];

  controller.setxy(0,0,2);
  controller.setxy(0,0,3);
  // Dummy reads needed after write and before read
  controller.readdata(2);
  controller.readdata(3);
  for (i=0;i<50;i++)
  {
    middle[0][i]=controller.readdata(2);
    middle[1][i]=controller.readdata(3);
  }

  for (j=0;j<3;j++)
  {
	  controller.setxy(0,j+1,0);
	  controller.setxy(0,j+1,1);
	  controller.setxy(0,j+1,2);
	  controller.setxy(0,j+1,3);
          // Dummy reads needed after write and before read
	  controller.readdata(0);
	  controller.readdata(1);
	  controller.readdata(2);
	  controller.readdata(3);
	  for (i=0;i<50;i++)
	  {
	    buf[0][i]=controller.readdata(0);
	    buf[1][i]=controller.readdata(1);
	    buf[2][i]=controller.readdata(2);
	    buf[3][i]=controller.readdata(3); 
	  }
	  controller.setxy(0,j,0);
	  controller.setxy(0,j,1);
	  controller.setxy(0,j,2);
	  controller.setxy(0,j,3);
	  for (i=0;i<50;i++)
	  {
	    controller.writedata(buf[0][i],0);
	    controller.writedata(buf[1][i],1);
	    controller.writedata(buf[2][i],2);
	    controller.writedata(buf[3][i],3);
	  }
  }

  controller.setxy(0,3,0);
  controller.setxy(0,3,1);
  controller.setxy(0,3,2);
  controller.setxy(0,3,3);
  for (i=0;i<50;i++)
  {
    controller.writedata(middle[0][i],0);
    controller.writedata(middle[1][i],1);
    controller.writedata(0,2);
    controller.writedata(0,3);
  }

}

void LcdLM234::invertChr(byte x, byte y)
{
  byte buf[5], i;

  setCursor(x,y);
  for (i=0;i<5;i++)
    buf[i]=read();
  setCursor(x,y);
  for (i=0;i<5;i++)
    writedata(buf[i] ^ 0xFF);
}

void LcdLM234::invertScr(byte x, byte y, byte length, byte height)
{
  byte i,j;

  for (j=y;j<height;j++)
    for (i=x;i<length;i++)
      invertChr(i,j);
}



