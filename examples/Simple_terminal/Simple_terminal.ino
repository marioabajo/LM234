/*----------------------------------------------------- 
Author:  Mario Abajo Duran --<marioabajo@gmail.com>
Date: 22:33:39/May/2013
Description:

-----------------------------------------------------*/
#include "LcdLM234.h"

/* INIT FUNCTION */
LcdLM234 lcd(19,18,2,3,4,6,5,8,7,10,9,12,11);


/**********************************
 GRAPHIC FUNCTIONS 
**********************************/

void demos()
{
  byte i,j;
  unsigned long t1;

  /* DEMO 1 */
  lcd.setCursor(0,0);
  for (j=0;j<8;j++)
  {
    for (i=0;i<100;i++)
    {
        lcd.writedata(i);
        //delay(10);
    }
  }
  delay (1000);
  
  /* DEMO 2 */
  lcd.cls();
  lcd.setCursor(0,0);
  for (i=0;i<160;i++)
    lcd.write(i);
  //delay(2000);
  t1=millis();
  for (i=0;i<8;i++)
  {
    lcd.scrollup(0,0,20,7);
  }
  Serial.println(millis()-t1);
  delay(1000);
  
  /* DEMO 3 */
  lcd.cls();
  lcd.autoscroll=false;
  for (i=0;i<20;i++)
  {
    lcd.setCursor(i,i*2/5);
    lcd.write(20+i);
    delay(50);
  }
  for (i=0;i<20;i++)
  {
    lcd.setCursor(i,7-(i*2/5));
    lcd.write(40+i);
    delay(50);
  }
  lcd.autoscroll=true;
  delay(1000);
  
  /* DEMO 4 */
  lcd.cls();
}

void put(byte c)
{
  byte i;
  
  switch (c)
  {
    case 8:   // BS backspace
      if (lcd.global_x>0)
        lcd.setCursor(lcd.global_x-1,lcd.global_y);
      else if (lcd.global_y>0)
        lcd.setCursor(lcd.max_x-1,lcd.global_y-1);
      break;

    case 127:   // BackSpace Key
      if (lcd.global_x>0)
      {
        lcd.setCursor(lcd.global_x-1,lcd.global_y);
        lcd.write(0);
        lcd.setCursor(lcd.global_x-1,lcd.global_y);
      }
      else if (lcd.global_y>0)
      {
        lcd.setCursor(lcd.max_x-1,lcd.global_y-1);
        lcd.write(0);
        lcd.setCursor(lcd.max_x-1,lcd.global_y-1);
      }
      break;
      
     case 7:   // Bell
       lcd.invertScr(0,0,lcd.max_x,lcd.max_y);
       lcd.invertScr(0,0,lcd.max_x,lcd.max_y);
     
     case 9:   // Tab  (4 spaces tabs)
       for(i=0;i<lcd.global_x%4;i++)
         lcd.write(0);
       break;

     case 10:  // NL new line
      if (lcd.global_y<lcd.max_y-1)
        lcd.setCursor(0,lcd.global_y+1);
      else
      {
        lcd.scrollup(0,0,lcd.max_x,lcd.max_y-1);
        lcd.setCursor(0,lcd.max_y-1);
      }
      break;
      
    case 12:  // clear screen
      lcd.cls();
      break;
        
    case 13:  // CR carrier return
      lcd.setCursor(0,lcd.global_y);
      break;
     
    default:
      lcd.write(c);
  }
}


void setup()
{
    // Init display
    Serial.begin(115200);
    lcd.autoscroll=true;

    demos();
    lcd.print(".-=#> WELLCOME <#=-.");
}

void loop()
{
  
  while (1)
    if (Serial.available())
      put(Serial.read());
}
