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
Timer loop_timer;
rIDTArray led_arr;
/*char Aleds[25] = {1,2,17,18,19,3,4,5,6,7,20,21,22,23,24,25,8,9,10,11,12,13,14,15,16};
char Aledb[120] = {1,2,3,17,18,19,33,34,35,49,50,51,65,66,67,81,82,83,97,98,99,113,114,115,
4,5,6,7,20,21,22,23,36,37,38,39,52,53,54,55,68,69,70,71,84,85,86,87,100,101,102,103,
8,9,10,11,24,25,26,27,40,41,42,43,56,57,58,59,72,73,74,75,88,89,90,91,104,105,106,107,116,117,118,119,
12,13,14,15,16,28,29,30,31,32,44,45,46,47,48,60,61,62,63,64,76,77,78,79,80,92,93,94,95,96,108,109,110,111,112,120};
*/
#define GSCLK 6 
#define LAT 2   // On Arduino Mega
#define CAM 8 // camera trigger

// Spi pins are needed to send out control bit (SPI only supports bytes)
#define SPI_MOSI 11 // 51 on mega, 22 on teensy2.0++
#define SPI_CLK 13 // 52 on mega, 21 on teensy2.0++


void setup() {
  pinMode(CAM, OUTPUT);
// Now set the GSCKGRB to an output and a 50% PWM duty-cycle
  // For simplicity all three grayscale clocks are tied to the same pin
  for (i=0; i<100; i++) {
    if (!Serial)
      delay(50);
  }
  pinMode(GSCLK, OUTPUT);
  pinMode(LAT,OUTPUT);

  // Adjust PWM timer (Specific to each microcontroller)
  //TCCR2B = TCCR2B & 0b11111000 | 0x01;

  // Set up clock pulse
  analogWriteFrequency(GSCLK, 1048576);
  analogWrite(GSCLK, 127);

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
  tlc.setAllLed(1240);
  tlc.updateLeds();
  delay(1000);
  tlc.setAllLed(0);
  tlc.updateLeds();
  delay(1000);

  // vars init
  led_arr.setTLC(&tlc);
  loop_timer.delay_until(0);
  i=0;
}

void loop() {
  tlc.setAllLed(0);
  //j=random(0, 5000);
  //for (j=0;j<120;j++)
  //tlc.setLed(Aledb[119]-1, random(0, 5000),random(0, 5000), random(0, 5000));
  
  led_arr.setLed(0,i ,0,65535,0);
  //led_arr.setLed(2,8*(i%2) ,0,0,65535);
  //tlc.setAllLed(10*(i), 10*(i), 1000);
  //tlc.setAllLed(3000);
  //tlc.setLed(Aledb[i]-1,0,0,65535);
  //tlc.setLed(Aledb[i]-1, 100,0,0);
  tlc.updateLeds();
  i++;
  i%=25;
  //if(i<10) i=128;
  //Serial.print(i);
  //Serial.println(" loop");
  digitalWrite(CAM, HIGH);
  delay(10);
  digitalWrite(CAM, LOW);
    
  loop_timer.delay_until(200);
}
