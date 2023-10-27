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
int i, j, mode;
unsigned char ring, c_get, restarted=false;
Timer loop_timer;
rIDTArray led_arr;

#define GSCLK1 5
#define GSCLK2 6 // same signal
#define GSCLK_PWM 248576 // PWM frequency
#define LAT 2   // On Arduino Mega
#define CAM 0 // camera trigger

// Spi pins are needed to send out control bit (SPI only supports bytes)
#define SPI_MOSI 11 // 51 on mega, 22 on teensy2.0++
#define SPI_CLK 13 // 52 on mega, 21 on teensy2.0++

#define CONSTANT 0
#define MASTER 1
#define INTERACTIVE 2


void setup() {
  // The baud rate setting with Serial.begin(baud) is ignored with USB serial
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
  delay(200);
  tlc.setAllLed(500);
  tlc.updateLeds();
  delay(1000);
  /*for (j=0;j<100;j++){
    tlc.setAllLed(30*j);
    tlc.updateLeds();
    delay(100);
  }*/
  tlc.setAllLed(0);
  tlc.updateLeds();
  delay(200);

  // vars init
  led_arr.setTLC(&tlc);
  i=0;
  ring=0;
  loop_timer.delay_until(0);
  mode = INTERACTIVE;
}

void loop() {
  tlc.setAllLed(0);
  Serial.write("[D]");
  Serial.println(i);
  if (i>=led_arr.LED_NUM[ring]) {
      i=0;
      ring=ring+1;
      if (ring >= 6) {
        ring = 0;
        restarted = true;
      }
  }
  led_arr.setLed(ring, i, 65535, 0, 0);
  //led_arr.setLed(1, 0, 65535,0, 0);
//  i = i % 4;
  //led_arr.setDPC(2, 20000, 0, 0);

  switch (mode) {
    case CONSTANT:
      tlc.updateLeds();
      while (1);
      break;
    case MASTER:
      delay(10);
      digitalWrite(CAM, HIGH);
      delay(10);
      digitalWrite(CAM, LOW); 
      tlc.updateLeds();
      loop_timer.delay_until(300);
      restarted = 0;
      i++;
      break;
    case INTERACTIVE:
      while (!Serial.available());
      while ((c_get=Serial.read()) != 'N'){
        Serial.write("[D] arduino get character ");
        Serial.print(c_get);
        Serial.write('\n');
        while (!Serial.available());
      }
      if (restarted) {
        tlc.setAllLed(0);
        tlc.updateLeds();
        Serial.write("[F]\n");
        restarted = false;
      } else {
        tlc.updateLeds();
        Serial.write("[N] ");
        Serial.print(ring);
        Serial.write(' ');
        Serial.print(i++);
        Serial.write('\n');
      }
  }
}
