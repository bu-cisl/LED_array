/*
 * TLC5955 Example Program
 * Zack Phillips, zkphil@berkeley.edu
 * Updated 11/16/2015
 */
 /*
 * Weiye Song, songweiye001@gmail.com
 * Updated 4/26/2020
 */
#include <TLC5955.h>
#include <SPI.h>

TLC5955 tlc;

#define GSCLK 6 // On Arduino Mega
#define LAT 2   // On Arduino Mega

// Spi pins are needed to send out control bit (SPI only supports bytes)
#define SPI_MOSI 11 // 51 on mega, 22 on teensy2.0++
#define SPI_CLK 13 // 52 on mega, 21 on teensy2.0++


void setup() {
// Now set the GSCKGRB to an output and a 50% PWM duty-cycle
  // For simplicity all three grayscale clocks are tied to the same pin
  pinMode(GSCLK, OUTPUT);
  pinMode(LAT,OUTPUT);

  // Adjust PWM timer (Specific to each microcontroller)
  //TCCR2B = TCCR2B & 0b11111000 | 0x01;

  // Set up clock pulse
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
  tlc.setFunctionData(false, true, true, false, true); // WORKS with fast update
  //tlc.setFunctionData(true, false, false, true, true);   // WORKS generally

  // set all brightness levels to max (127)
  int currentVal = 127;
  tlc.setBrightnessCurrent(currentVal,currentVal,currentVal);

  // Update Control Register
  tlc.updateControl();

  // Update the GS register (ideally LEDs should be dark up to here)
  tlc.setAllLeds(0);
  tlc.updateLeds();
  

}

void loop() {
 tlc.setAllLeds(0);
  int led=0;
  int colorr=65535;
  int colorg=0;
  int colorb=0;
  int16_t mDelay = 100;

  int16_t dimMax = 60000;
  int16_t dimDelta = 4000;
  uint16_t dimVal =65535;
  char Aled[65]= {0,1,16,17,18,2,3,4,5,6,19,20,21,22,23,24,7,8,9,10,11,12,13,14,15,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71};

  char mark=0;
  char flag=0;
  char rxdata=0;
  int numpostion=0;
  int firsti =0;
  int numdata[20];
  int temp;
  String rx_buffer = "";
  Serial.setTimeout(1);
  Serial.begin(115200); 
 delay(100);
 rx_buffer=Serial.readString();
while(1)
{    
  
  while ((Serial.available() > 0)&&(flag==0))  
    {
        rxdata = char(Serial.read());
        if(rxdata ==  '%')
        {
          rx_buffer += rxdata;
          flag=1;
        }
        else
        rx_buffer += rxdata;
       
        
    }
  //  if (comdata.length() > 0)
       if(flag==1)
    {   flag=0;
        Serial.println(rx_buffer);
        while(Serial.read() >= 0){}
        
    if(rx_buffer[0]=='x')
       {tlc.setAllLed(0,0,0);
       tlc.updateLeds();}
    else if (rx_buffer[0]=='f' && rx_buffer[1]=='f' )
     { 
       tlc.setAllLed(colorr,colorg,colorb);
       tlc.updateLeds();
//       for(int i=0;i<65;i++)
//        {
//          tlc.setLed(Aled[i],colorr,colorg,colorb);
//          delay(10);
//       tlc.updateLeds();
//       }
     }
    else if (rx_buffer[0]=='b' && rx_buffer[1]=='f' )
     { tlc.setAllLed(0,0,0);
        for(int i=0;i<25;i++)
        tlc.setLed(Aled[i],colorr,colorg,colorb);
       tlc.updateLeds();
     }
         else if (rx_buffer[0]=='b' && rx_buffer[1]=='l'&& rx_buffer[2]=='f' )
     { tlc.setAllLed(0,0,0);
        for(int i=0;i<6;i++)
        tlc.setLed(Aled[i],colorr,colorg,colorb);
         for(int i=9;i<18;i++)
        tlc.setLed(Aled[i],colorr,colorg,colorb);
       tlc.updateLeds();
     }
    else if (rx_buffer[0]=='b' && rx_buffer[1]=='r'&& rx_buffer[2]=='f' )
     { tlc.setAllLed(0,0,0);
        for(int i=0;i<2;i++)
        tlc.setLed(Aled[i],colorr,colorg,colorb);
        for(int i=5;i<9;i++)
        tlc.setLed(Aled[i],colorr,colorg,colorb);
         for(int i=17;i<25;i++)
        tlc.setLed(Aled[i],colorr,colorg,colorb);
        tlc.setLed(Aled[9],colorr,colorg,colorb);
       tlc.updateLeds();
     }
         else if (rx_buffer[0]=='d' && rx_buffer[1]=='f' )
     { tlc.setAllLed(0,0,0);
        for(int i=25;i<65;i++)
        tlc.setLed(Aled[i],colorr,colorg,colorb);
       tlc.updateLeds();
     }
      else if (rx_buffer[0]=='d' && rx_buffer[1]=='l'&& rx_buffer[2]=='f')
     { tlc.setAllLed(0,0,0);
        for(int i=25;i<38;i++)
        tlc.setLed(Aled[i],colorr,colorg,colorb);
         for(int i=49;i<58;i++)
        tlc.setLed(Aled[i],colorr,colorg,colorb);
       tlc.updateLeds();
     }
     else if (rx_buffer[0]=='d' && rx_buffer[1]=='r'&& rx_buffer[2]=='f')
     { tlc.setAllLed(0,0,0);
        for(int i=37;i<49;i++)
        tlc.setLed(Aled[i],colorr,colorg,colorb);
         for(int i=57;i<65;i++)
        tlc.setLed(Aled[i],colorr,colorg,colorb);
        tlc.setLed(Aled[25],colorr,colorg,colorb);
        tlc.setLed(Aled[49],colorr,colorg,colorb);
       tlc.updateLeds();
     }
    else if (rx_buffer[0]=='s' && rx_buffer[1]=='c'&& rx_buffer[2]=='.' )
        {  
            if (rx_buffer[3]=='r')
            {
            colorr=65535;
            colorg=0;
            colorb=0;}
            if (rx_buffer[3]=='g')
            {colorr=0;
            colorg=65535;
            colorb=0;}
            if (rx_buffer[3]=='b')
            {colorr=0;
            colorg=0;
            colorb=65535;}
            if (rx_buffer[3]=='w')
            {colorr=65535;
            colorg=65535;
            colorb=65535;}
         }
       else if (rx_buffer[0]=='l' && rx_buffer[1]=='.' )
            { numpostion=0;
              numdata[numpostion]=0;
              
              for(int i=0;i<rx_buffer.length();i++)
              {
                if(rx_buffer[i] == '.')
                            {
                             firsti=i;
                             break;
                            }
              }
              for(int i=firsti+1;i<rx_buffer.length();i++)
              {
                    if(((rx_buffer[i]>('0'-1))&&(rx_buffer[i]<('9'-1)))||(rx_buffer[i]='.'))
                    {
                     if(rx_buffer[i] == '.')
                            {
                              numpostion++;
                              numdata[numpostion]=0;
                            }
                            else
                             {
                                 numdata[numpostion] = numdata[numpostion] * 10 + (rx_buffer[i] - '0');
                              } 
                    }
                    else
                    i=     rx_buffer.length()    ;   
               }
               tlc.setAllLed(0,0,0);
             
              for(int i=0;i<numpostion+1;i++)
              {            
              tlc.setLed(Aled[numdata[i]-1],colorr,colorg,colorb);
              }
              memset( numdata, 0, sizeof(numdata) );
              tlc.updateLeds();
              
              }
        else if (rx_buffer[0]=='l' && rx_buffer[1]=='b' && rx_buffer[2]=='.' )
            { numpostion=0;
              numdata[numpostion]=0;
              for(int i=0;i<rx_buffer.length();i++)
              {
                if(rx_buffer[i] == '.')
                            {
                              firsti=i;
                             break;
                            }
              }
              for(int i=firsti+1;i<rx_buffer.length();i++)
              {
                     if(rx_buffer[i] == '.')
                            {
                              numpostion++;
                              numdata[numpostion]=0;
                            }
                            else
                             {
                numdata[numpostion] = numdata[numpostion] * 10 + (rx_buffer[i] - '0');
                              }             
               }
               tlc.setAllLed(0,0,0);
             
              for(int i=0;i<numpostion+1;i++)
              {            
              tlc.setLed(Aled[numdata[i]-1],colorr,colorg,colorb);
              }
              memset( numdata, 0, sizeof(numdata) );
              tlc.updateLeds();
              
              }
            else if (rx_buffer[0]=='l' && rx_buffer[1]=='d' && rx_buffer[2]=='.' )
            { numpostion=0;
              numdata[numpostion]=0;
              for(int i=0;i<rx_buffer.length();i++)
              {
                if(rx_buffer[i] == '.')
                            {
                              firsti=i;
                             break;
                            }
              }
              for(int i=firsti+1;i<rx_buffer.length();i++)
              {
                     if(rx_buffer[i] == '.')
                            {
                              numpostion++;
                              numdata[numpostion]=0;
                            }
                            else
                             {
                numdata[numpostion] = numdata[numpostion] * 10 + (rx_buffer[i] - '0');
                              }             
               }
               tlc.setAllLed(0,0,0);
             
              for(int i=0;i<numpostion+1;i++)
              {            
              tlc.setLed(Aled[numdata[i]+24],colorr,colorg,colorb);
              }
              memset( numdata, 0, sizeof(numdata) );
              tlc.updateLeds();
              
              }
               else if (rx_buffer[0]=='s' && rx_buffer[1]=='c' && rx_buffer[2]=='a'&& rx_buffer[3]=='n'&& rx_buffer[4]=='.')
               
               {
                   numpostion=0;
              numdata[numpostion]=0;
              for(int i=0;i<rx_buffer.length();i++)
              {
                if(rx_buffer[i] == '.')
                            {
                              firsti=i;
                             break;
                            }
              }
              for(int i=firsti+1;i<rx_buffer.length();i++)
                  {
                     if(rx_buffer[i] == '.')
                            {
                              numpostion++;
                              numdata[numpostion]=0;
                            }
                            else
                             {
                              numdata[numpostion] = numdata[numpostion] * 10 + (rx_buffer[i] - '0');
                              }             
               }
                  mDelay = numdata[0] ;
                  tlc.setAllLed(0,0,0);
                for(int i=0;i<65;i++)
                      {
                      tlc.setLed(Aled[i],colorr,colorg,colorb);
                      tlc.updateLeds();
                      tlc.setLed(Aled[i],0,0,0);
                      delay(mDelay);
                      }
                
                }
  else if (rx_buffer[0]=='s' && rx_buffer[1]=='c' && rx_buffer[2]=='a'&& rx_buffer[3]=='n'&& rx_buffer[4]=='d'&& rx_buffer[5]=='.')
               
               {
                   numpostion=0;
              numdata[numpostion]=0;
              for(int i=0;i<rx_buffer.length();i++)
              {
                if(rx_buffer[i] == '.')
                            {
                              firsti=i;
                             break;
                            }
              }
              for(int i=firsti+1;i<rx_buffer.length();i++)
                  {
                     if(rx_buffer[i] == '.')
                            {
                              numpostion++;
                              numdata[numpostion]=0;
                            }
                            else
                             {
                              numdata[numpostion] = numdata[numpostion] * 10 + (rx_buffer[i] - '0');
                              }             
               }
                  mDelay = numdata[0] ;
                  tlc.setAllLed(0,0,0);
                for(int i=25;i<65;i++)
                      {
                      tlc.setLed(Aled[i],colorr,colorg,colorb);
                      tlc.updateLeds();
                      tlc.setLed(Aled[i],0,0,0);
                      delay(mDelay);
                      }
                
                }
                  else if (rx_buffer[0]=='s' && rx_buffer[1]=='c' && rx_buffer[2]=='a'&& rx_buffer[3]=='n'&& rx_buffer[4]=='b'&& rx_buffer[5]=='.')
               
               {
                   numpostion=0;
              numdata[numpostion]=0;
              for(int i=0;i<rx_buffer.length();i++)
              {
                if(rx_buffer[i] == '.')
                            {
                              firsti=i;
                             break;
                            }
              }
              for(int i=firsti+1;i<rx_buffer.length();i++)
                  {
                     if(rx_buffer[i] == '.')
                            {
                              numpostion++;
                              numdata[numpostion]=0;
                            }
                            else
                             {
                              numdata[numpostion] = numdata[numpostion] * 10 + (rx_buffer[i] - '0');
                              }             
               }
                  mDelay = numdata[0] ;
                  tlc.setAllLed(0,0,0);
                for(int i=0;i<25;i++)
                      {
                      tlc.setLed(Aled[i],colorr,colorg,colorb);
                      tlc.updateLeds();
                      tlc.setLed(Aled[i],0,0,0);
                      delay(mDelay);
                      }
                
                }


    }

       
//  tlc.setLed(Aled[led],dimVal,0,0);
//    tlc.updateLeds();
//        delay(mDelay);
////        Serial.print("ab\n");
//    tlc.setLed(Aled[led],0,0,0);
//    
//    tlc.updateLeds();
//    delay(mDelay);
//   tlc.setLed(Aled(led),0,dimVal,0);
//    tlc.updateLeds();
//         delay(mDelay);
//         Serial.print("ac\n");
//         tlc.setLed(led,0,0,dimVal);
//    tlc.updateLeds();
//        delay(mDelay);
//        tlc.setLed(led,0,0,0);
//    
//    tlc.updateLeds();
//    delay(mDelay);
//        tlc.setAllLed(1000,0,0);
//    tlc.updateLeds();
//        delay(mDelay);
//                tlc.setAllLed(0,1000,0);
//    tlc.updateLeds();
//        delay(mDelay);
//                tlc.setAllLed(0,0,1000);
//    tlc.updateLeds();
//        delay(mDelay);
//         tlc.setAllLed(0,0,0);
//    tlc.updateLeds();
//        delay(mDelay);
// led++;
// if(led ==65)
// led =0;
  }
  // PWM Dimming
  for (uint16_t dimVal = 0; dimVal <= dimMax; dimVal += dimDelta)
  {
 //   tlc.setAllLed(0);
    tlc.setLed(led,dimVal,0,0);
    tlc.updateLeds();
  }

    delay(mDelay);
  for (uint16_t dimVal = dimMax; dimVal >= dimDelta; dimVal -= dimDelta)
  {
    
 //   tlc.setAllLed(0);
    tlc.setLed(led,0,dimVal,0);
    tlc.updateLeds();
  }
  delay(mDelay);
}
