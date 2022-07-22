/*
 * some useful tools by Jiabei
 */

#include "utils.h"

unsigned long prev_us = 0;
unsigned long delay_warning = 1;

// Timer::Timer()


void Timer::delay_until(float ms)
{
  bool late = 1;
  unsigned long us = ms * 1000;

  // delay using empty loop until OK
  while(micros() - _prev_us < us) late = 0;
    
  if (late){
    if (us && _delay_warning){
      Serial.print(F("Trying to delay "));
      Serial.print(us);
      Serial.print(F(" us but already passed "));
      Serial.print(micros() - _prev_us);
      Serial.println(F(" us!"));
      _delay_warning--;
    }
    _prev_us = micros();
  }
  else
    _prev_us += us;
}

void rIDTArray::setLed(uint16_t ring, uint16_t led, uint16_t red, uint16_t green, uint16_t blue)
{
  uint16_t ledNum = -1;
  if (_tlc == NULL)
    return;
  if (ring > 2)  // judge DF (big)
    switch(ring) {
      case 6:  led += 32;
      case 5:  led += 28;
      case 4:  led += 24;
      default: if (led<120) ledNum = _LED_DF[led];
    }
  else  // judge BF (small)
    switch(ring) {
      case 2:  led += 8;
      case 1:  led += 1;
      default: if (led<25) ledNum = _LED_BF[led] + 128;
    };
  // update only if valid
  if (ledNum != -1)
    _tlc->setLed(ledNum - 1, red, green, blue);
}

void rIDTArray::setLed(uint16_t ring, uint16_t led, uint16_t rgb)
{
  setLed(ring, led, rgb, rgb, rgb);
}

void rIDTArray::setTLC(TLC5955 *tlc)
{
  _tlc = tlc;
}

void trigger()
{
  
}