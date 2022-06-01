/*
 * TLC5955 Example Program
 * Zack Phillips, zkphil@berkeley.edu
 * Updated 11/16/2015
 */
 /*
 * Weiye Song, songweiye001@gmail.com
 * Updated 4/26/2020
 */
#include "TLC5955.h"
#include "SPI.h"
#include "utils.h"

TLC5955 tlc;
int i, j;
unsigned char ring;
Timer loop_timer;
rIDTArray led_arr;
/*char Aleds[25] = {1,2,17,18,19,3,4,5,6,7,20,21,22,23,24,25,8,9,10,11,12,13,14,15,16};
char Aledb[120] = {1,2,3,17,18,19,33,34,35,49,50,51,65,66,67,81,82,83,97,98,99,113,114,115,
4,5,6,7,20,21,22,23,36,37,38,39,52,53,54,55,68,69,70,71,84,85,86,87,100,101,102,103,
8,9,10,11,24,25,26,27,40,41,42,43,56,57,58,59,72,73,74,75,88,89,90,91,104,105,106,107,116,117,118,119,
12,13,14,15,16,28,29,30,31,32,44,45,46,47,48,60,61,62,63,64,76,77,78,79,80,92,93,94,95,96,108,109,110,111,112,120};
*/
uint16_t p0[ 1]={62000};
uint16_t p1[ 8]={59600, 60700, 57800, 61200, 62200, 58200, 58900, 55500};
uint16_t p2[16]={58600, 58000, 57100, 55100, 58800, 58600, 59800, 57700,
                 65100, 65300, 61200, 63500, 62000, 57700, 63900, 57700};
uint16_t p3[24]={28100, 24700, 28400, 27000, 29900, 26700, 30100, 27900,
                 27300, 31600, 32200, 29300, 31000, 33600, 34200, 31600,
                 27300, 30400, 30700, 29600, 25600, 28400, 27000, 25300};
uint16_t p4[28]={36400, 29100, 30500, 29100, 31800, 30100, 35000, 37100,
                 34000, 38800, 33600, 39900, 42300, 41200, 42600, 37400,
                 40800, 39200, 34000, 38400, 38800, 39500, 36000, 35000,
                 32600, 29400, 32600, 30800};
uint16_t p5[32]={47200, 44500, 38200, 37000, 40600, 41000, 36000, 55000,
                 49700, 50700, 52500, 48900, 48100, 49700, 54600, 51600,
                 52000, 58500, 56000, 56400, 49700, 45000, 56100, 46800,
                 53400, 45400, 48000, 43400, 42600, 34800, 41400, 39000};
uint16_t p6[36]={65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
                 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
                 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
                 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
                 65535, 65535, 65535, 65535};
uint16_t* LED_POWER[7]={p0,p1,p2,p3,p4,p5,p6};

#define GSCLK1 5
#define GSCLK2 6 // same signal
#define GSCLK_PWM 1048576*1 // PWM frequency
#define LAT 2   // On Arduino Mega
#define CAM 0 // camera trigger

// Spi pins are needed to send out control bit (SPI only supports bytes)
#define SPI_MOSI 11 // 51 on mega, 22 on teensy2.0++
#define SPI_CLK 13 // 52 on mega, 21 on teensy2.0++


void setup() {
  
  // wait for serial connection (set timeout)
  for (i=0; i<30; i++) {
    if (!Serial)
      delay(100);
  }
  
  pinMode(CAM, OUTPUT);
  pinMode(GSCLK1, OUTPUT);
  pinMode(GSCLK2, OUTPUT);
  pinMode(LAT,OUTPUT);
  //TCCR2B = TCCR2B & 0b11111000 | 0x01;

  // Adjust PWM timer (Specific to each microcontroller)
  // Set up clock pulse
  analogWriteFrequency(GSCLK1, GSCLK_PWM);
  analogWriteFrequency(GSCLK2, GSCLK_PWM);
  // Now set the GSCKGRB to an output and a 50% PWM duty-cycle
  analogWrite(GSCLK1, 127);
  analogWrite(GSCLK2, 127);

  // The library does not ininiate SPI for you, so as to prevent issues with other SPI libraries
  SPI.begin();

  // init(GSLAT pin, XBLNK pin, default grayscale values for all LEDS)
  tlc.init(LAT, SPI_MOSI,SPI_CLK);

  // We must set dot correction values, so set them all to the brightest adjustment
  tlc.setAllDcData(127);

  // Set Max Current Values (see TLC5955 datasheet)
  tlc.setMaxCurrent(4,4,4);

  // Set Function Control Data Latch values. See the TLC5955 Datasheet for the purpose of this latch.
  //tlc.setFunctionData(false, true, true, false, true); // WORKS with fast update
  tlc.setFunctionData(true, false, false, true, true);  // WORKS generally

  // set all brightness levels to max (127)
  int currentVal = 127;
  tlc.setBrightnessCurrent(currentVal,currentVal,currentVal);

  // Update Control Register
  tlc.updateControl();

  // Update the GS register (ideally LEDs should be dark up to here)
  tlc.setRgbPinOrder(0,1,2);
  tlc.setAllLed(0);
  tlc.updateLeds();
  delay(1000);
  tlc.setAllLed(500);
  tlc.updateLeds();
  delay(2000);
  /*for (j=0;j<100;j++){
    tlc.setAllLed(30*j);
    tlc.updateLeds();
    delay(100);
  }*/
  tlc.setAllLed(0);
  tlc.updateLeds();
  delay(2000);

  // vars init
  led_arr.setTLC(&tlc);
  i=0;
  ring=0;
  loop_timer.delay_until(0);
}

void loop() {
  tlc.setAllLed(0);
  if (i>=led_arr.LED_NUM[ring]) {
      i=0;
      ring=(ring+1)%6;
  }
  
  led_arr.setLed(ring, i, LED_POWER[ring][i],0, 0);
  //led_arr.setLed(ring, i, LED_POWER[ring][i],0, 0);
  //led_arr.setDPC(led_arr.TOP, LED_POWER[ring][i],0, 0);
  i++;
  tlc.updateLeds();
  
  delay(10);
  digitalWrite(CAM, HIGH);
  delay(10);
  digitalWrite(CAM, LOW);
  loop_timer.delay_until(400);
}
