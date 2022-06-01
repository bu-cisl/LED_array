/*
Some useful tools
  - timer
  - high-level LED control functions (specified for our rIDT setup)
Original author: Jiabei Zhu <zjb@bu.edu>
Last modified: 05/31/2022
*/

#ifndef LED_utils_h
#define LED_utils_h

#include <Arduino.h>
#include "TLC5955.h"

#define UTILS_DEBUG 1

class Timer
{
  public:
    // Timer();
    void delay_until(float ms);
  private:
    unsigned long _prev_us = 0;
    unsigned long _delay_warning = 3;
};


class rIDTArray
{
  private:
    static constexpr uint8_t _LED_BF[25] = {   // ring 0-2, LED num 1/8/16
      1,
      2,17,18,19, 3,4,5,6,
      7,20,21,22, 23,24,25,8, 9,10,11,12, 13,14,15,16
    };
    static constexpr uint8_t _LED_DF[120] = {  // ring 3-6, LED num 24/28/32/36
      1,115,114,113,  99,98,97,83,     82,81,67,66,  65,51,50,49, 35,34,33,19, 18,17,3,2,
      4,103,102,101,  100,87,86,85,    84,71,70,69,  68,55,54,53, 52,39,38,37, 36,23,22,21, 20,7,6,5,
      8,119,118,117,  116,107,106,105, 104,91,90,89, 88,75,74,73, 72,59,58,57, 56,43,42,41, 40,27,26,25, 24,11,10,9,
      12,120,112,111, 110,109,108,96,  95,94,93,92,  80,79,78,77, 76,64,63,62, 61,60,48,47, 46,45,44,32, 31,30,29,28, 16,15,14,13
    };
    /* reversed order
     * char _LED_DF[120] = {  // ring 3-6, LED num 24/28/32/36
      1,2,3,17,    18,19,33,34, 35,49,50,51, 65,66,67,81, 82,83,97,98, 99,113,114,115,
      4,5,6,7,     20,21,22,23, 36,37,38,39, 52,53,54,55, 68,69,70,71, 84,85,86,87, 100,101,102,103,
      8,9,10,11,   24,25,26,27, 40,41,42,43, 56,57,58,59, 72,73,74,75, 88,89,90,91, 104,105,106,107, 116,117,118,119,
      12,13,14,15, 16,28,29,30, 31,32,44,45, 46,47,48,60, 61,62,63,64, 76,77,78,79, 80,92,93,94,     95,96,108,109, 110,111,112,120
    };
    char _LED_BF[25] = {   // ring 0-2, LED num 1/8/16
      1,
      2,6,5,4,    3,19,18,17,
      7,16,15,14, 13,12,11,10, 9,8,25,24, 23,22,21,20,
    };*/
    TLC5955 *_tlc = NULL;
    
  public:
    enum Direction {TOP, BOTTOM, LEFT, RIGHT};
    static constexpr uint16_t LED_NUM[7] = {1, 8, 16, 24, 28, 32, 36};
    void setTLC(TLC5955 *tlc);
    void setLed(uint16_t ring, uint16_t ledNum, uint16_t red, uint16_t green, uint16_t blue);
    void setLed(uint16_t ring, uint16_t led, uint16_t rgb);
    void setDPC(Direction d, uint16_t red, uint16_t green, uint16_t blue);
};

//void trigger(uint16_t );
//void trigger(unsigned long ms);

#endif
