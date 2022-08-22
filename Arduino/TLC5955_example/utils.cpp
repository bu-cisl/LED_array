/*
Some useful tools
  - timer
  - high-level LED control functions (specified for our rIDT setup)
Original author: Jiabei Zhu <zjb@bu.edu>
Last modified: 05/31/2022
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
  while (micros() - _prev_us < us) late = 0;

  if (late) {
    if (us && _delay_warning) {
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

constexpr uint16_t rIDTArray::LED_NUM[7];
constexpr uint8_t rIDTArray::_LED_BF[25];
constexpr uint8_t rIDTArray::_LED_DF[120];

void rIDTArray::setLed(uint16_t ring, uint16_t led, uint16_t red, uint16_t green, uint16_t blue)
{
  uint16_t ledNum = -1;
  float power_r = RED_LED_POWER[ring][led];
  if (_tlc == NULL)
    return;
  if (ring > 2)  // judge DF (big)
    switch (ring) { // Note: there is NO break in each case, so ring6 led+=32+28+24, etc.
      case 6:  led += 32;
      case 5:  led += 28;
      case 4:  led += 24;
      default: if (led < 120) ledNum = _LED_DF[led];
    }
  else  // judge BF (small)
    switch (ring) {
      case 2:  led += 8;
      case 1:  led += 1;
      default: if (led < 25) ledNum = _LED_BF[led] + 128;
    };
  // update only if valid
  if (ledNum != -1)
    _tlc->setLed(ledNum - 1, (uint16_t)(power_r * red / 65535), green, blue);
}

void rIDTArray::setLed(uint16_t ring, uint16_t led, uint16_t rgb)
{
  setLed(ring, led, rgb, rgb, rgb);
}

void rIDTArray::setDPC(Direction d, uint16_t red, uint16_t green, uint16_t blue)
{
  uint8_t i;
  switch (d) {
  case TOP:
    for (i = 1; i < 4; i++)
      setLed(1, i, red, green, blue);
    for (i = 1; i < 8; i++)
      setLed(2, i, red, green, blue);
    break;
  case BOTTOM:
    for (i = 5; i < 8; i++)
      setLed(1, i, red, green, blue);
    for (i = 9; i < 16; i++)
      setLed(2, i, red, green, blue);
    break;
  case LEFT:
    for (i = 3; i < 6; i++)
      setLed(1, i, red, green, blue);
    for (i = 5; i < 12; i++)
      setLed(2, i, red, green, blue);
    break;
  case RIGHT:
    for (i = 7; i != 2; i=(i+1)%8)
      setLed(1, i, red, green, blue);
    for (i = 13; i != 4; i=(i+1)%16)
      setLed(2, i, red, green, blue);
  }
//  if (d == BOTTOM || d == LEFT) {
//    for (i = 5; i < 6; i++)
//      setLed(1, i, red, green, blue);
//    for (i = 9; i < 12; i++)
//      setLed(2, i, red, green, blue);
//  }
//  if (d == BOTTOM || d == RIGHT) {
//    for (i = 6; i < 8; i++)
//      setLed(1, i, red, green, blue);
//    for (i = 12; i < 16; i++)
//      setLed(2, i, red, green, blue);
//  }
}

void rIDTArray::setTLC(TLC5955 *tlc)
{
  _tlc = tlc;
}

//void trigger()
//{
//
//}
