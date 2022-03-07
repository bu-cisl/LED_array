/*
 * some useful tools by Jiabei
 */

#include "utils.h"

unsigned long prev_us = 0;
unsigned long delay_warning = 1;

void delay_until(unsigned long us)
{
  bool late = 1;

  // delay using empty loop until OK
  while(micros() - prev_us < us) late = 0;
    
  if (late){
    if (us && delay_warning){
      Serial.print(F("Try to delay but already passed "));
      Serial.print(micros() - prev_us);
      Serial.println(F(" us"));
      delay_warning--;
    }
    prev_us = micros();
  }
  else
    prev_us += us;
}
