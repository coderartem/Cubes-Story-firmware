#include <neopixel.h>
#include <adafruit-drv2605-photon.h>
#include "Particle.h"
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

SYSTEM_MODE(AUTOMATIC);

#define PIXEL_PIN D2
#define PIXEL_COUNT 4
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
Adafruit_MMA8451 Acc = Adafruit_MMA8451();
Adafruit_DRV2605 drv;

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Light Level (should be higher in brighter room) max value 4500
int LightLevel=3500;
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
uint8_t stage = 0;
uint8_t st1 = 1;
uint8_t st2 = 0;
float xx = 0.0;
float yy = 0.0;
uint32_t Shake = 0;
uint8_t Gladit = 0;

uint8_t cs = 0;

bool turnOn = true;
bool angry = false;
bool firstMeet = false;
bool bzz = true;


//-----------------
//pins
int Gerkon = A0;
int Speaker = D4 ;
int MainV = D6;
int Buzz = D3;
int IR = D5;
int PhCell = A5;

void setup()
{
  //-------------------------------------------  активизация вибратора
    drv.begin();
    drv.selectLibrary(1);
    drv.setMode(DRV2605_MODE_INTTRIG);
  //--------------------------------------------
  //--------------------------------------------активизация акселерометра (датчик движения самого себя)
    if (! Acc.begin()) {
      while (1);
    }
    Acc.setRange(MMA8451_RANGE_2_G);

    pinMode(MainV,OUTPUT);
    pinMode(Speaker,OUTPUT);
    pinMode(PhCell,INPUT);
    pinMode(IR,INPUT);
    pinMode(Gerkon,INPUT);
    digitalWrite(MainV,0);
    digitalWrite(Speaker,0);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  delay(5000);

}

void loop()
{
///*
if(digitalRead(IR)==1 && turnOn){  //блок включения
    digitalWrite(MainV,1);
    strip.show();
    stage=1;
    turnOn=false;
    delay(500);

    Acc.read();
    xx=Acc.x;
    yy=Acc.y;
}                                   //блок включения
//*/

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
switch (stage){

  case 1:         // первый стэйдж

    switch (st1){

      case 1:         //запуск мод
        LedColor(cs);
        st1=2;
      break;

      case 2:    //waiting mode
        cs=1;
        LedColor(cs);
        firstMeet=true;
      break;

      case 3:         // таращение
        LedColor(cs);
        st1=0;
      break;
  }
//-----------------------------------------------
//поворот боя и свист

    if(firstMeet){
      Acc.read();

      if(st1==2 && abs(xx-Acc.x)>100 || abs(yy-Acc.y)>100){
        cs=2;
        st1=3;
      }
      xx=Acc.x;
      yy=Acc.y;
      delay(300);
    }
//----------------------------------------------

    if(st1==0 && digitalRead(Gerkon)==1){
      stage=2;
      st2=1;
      angry=false;
      Shake=0;
    }

//----------------------------------------------
//getting angry
    if(angry){
      sensors_event_t evt;
      Acc.getEvent(&evt);

      if(abs(evt.acceleration.x)>1 || abs(evt.acceleration.y)>1){
        Shake++;
      }

    if(Shake>7){
      stage=3;
      Shake=0;
      angry=false;
    }
    delay(300);
    }
  break;

//><>><><><><><><><><>><><><><><><><
  case 2:

      switch (st2){

        case 1:       //boy start to talk
          delay(100);
          digitalWrite(Speaker,1);
          LedColor(5);
          tone(Buzz,random(3,5)*100);
          delay(1000);
          digitalWrite(Speaker,0);
          st2=2;
        break;

        case 2:      // boy talking
          LedColor(5);
          tone(Buzz,random(4,6)*100);
          delay(random(2,4)*20);
          LedColor(6);
          noTone(Buzz);
          delay(random(1,2)*10);
        break;

          case 3:      //girl start to talk
          LedColor(6);
          st2=4;
        break;

        case 4:     //boy waiting to start to talk
          delay(random(4,7)*1000);
          st2=1;
        break;
        }

        if(st2==2 && analogRead(PhCell)>3500){
          st2=3;
        }

        if(digitalRead(Gerkon)==0){
          LedColor(6);
          delay(100);
          stage=1;
          st1=3;
          cs = 3;
          angry=true;
          noTone(Buzz);
        }


  break;
//><>><><><><><><><><>><><><><><><><
  case 3:
    LedColor(4);        // глаза горят
    tone(Buzz,random(4,6)*100);
    drv.setWaveform(0, 52);
    drv.setWaveform(1, 0);
    drv.go();
    delay(random(6,9)*100);

    if(digitalRead(IR)==1){
      Gladit++;
    }

    LedColor(6);   // глаза не горят
    noTone(Buzz);
    delay(random(3,5)*100);


    if(digitalRead(IR)==1){  // дубляж
      Gladit++;
    }

    if(Gladit>16){
      stage=1;
      st1=3;
      cs = 1;
      Gladit=0;
      noTone(Buzz);
      break;
    }
  break;
  }
}
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

void LedColor (uint8_t css){      //задание цвета лампочкам весь блок до конца

switch(css){

  case 0:         // wake up
    for(uint8_t i =0; i<4;i++){
        strip.clear();
        strip.show();
        delay(100);
        strip.setPixelColor(0,strip.Color(0,0,255));
        strip.setPixelColor(3,strip.Color(0,0,255));
        strip.show();
        delay(200);
        strip.clear();
        delay(100);
        strip.show();
        strip.setPixelColor(1,strip.Color(255,0,0));
        strip.setPixelColor(2,strip.Color(255,0,0));
        strip.show();
        delay(200);
      }

      for(uint8_t i =0; i<7;i++){
        strip.clear();
        delay(100);
        strip.show();
        strip.setPixelColor(0,strip.Color(0,0,255));
        strip.setPixelColor(1,strip.Color(255,0,0));
        strip.show();
        delay(200);
        strip.clear();
        delay(100);
        strip.show();
        strip.setPixelColor(2,strip.Color(255,0,0));
        strip.setPixelColor(3,strip.Color(0,0,255));
        strip.show();
        delay(200);
      }

      for(uint8_t i = 0; i<7;i++){
        strip.clear();
        strip.show();
        delay(200);
        for(uint8_t g = 0; g<4;g++){
          strip.setPixelColor(g,strip.Color(0,0,255));
          }
        strip.show();
        delay(300);
      }
    break;

    case 1:       // медленное мигание / waiting mode
      strip.clear();
      strip.show();
      delay(400);
      for(uint8_t g = 0; g<4;g++){
        strip.setPixelColor(g,strip.Color(0,0,255));
        }
      strip.show();
      delay(1500);
    break;

    case 2:         //присвист

        for(uint8_t i =0; i<4;i++){
          for(uint8_t g = 0; g<4;g++){
            strip.setPixelColor(g,strip.Color(0,0,255));
            }
          strip.show();
          delay(800);
          strip.clear();
          strip.show();
          for(uint8_t g = 0; g<2;g++){
            strip.setPixelColor(g,strip.Color(0,0,255));
            }
          strip.show();
          delay(800);
          strip.clear();
          strip.show();

          if(bzz){
            Buzzer();
            bzz=false;
          }
        }
        for(uint8_t g = 1; g<3;g++){
          strip.setPixelColor(g,strip.Color(127,0,127));
        }
        strip.show();
        firstMeet=false;
    break;

    case 3:
      for(uint8_t g = 0; g<4;g++){
        strip.setPixelColor(g,strip.Color(0,0,255));
      }
      strip.show();
    break;

    case 4:
      strip.clear();
      delay(100);
      strip.show();
      for(uint8_t g = 0; g<4;g++){
        strip.setPixelColor(g,strip.Color(0,255,0));
      }
      delay(100);
      strip.show();
    break;

    case 5:
      for(uint8_t g = 0; g<4;g++){
        strip.setPixelColor(g,strip.Color(255,0,0));
      }
      delay(100);
      strip.show();
    break;

    case 6:
      strip.clear();
      delay(100);
      strip.show();
    break;

}
}

void Buzzer (){

      tone(Buzz,100);
      delay(20);
      tone(Buzz,150);
      delay(20);
      tone(Buzz,200);
      delay(20);
      tone(Buzz,250);
      delay(20);
      tone(Buzz,350);
      delay(20);
      tone(Buzz,400);
      delay(20);
      tone(Buzz,450);
      delay(20);
      tone(Buzz,500);
      delay(20);
      tone(Buzz,550);
      delay(20);
      tone(Buzz,600);
      delay(20);
      tone(Buzz,650);
      delay(20);
      tone(Buzz,700);
      delay(20);
      tone(Buzz,750);
      delay(20);
      tone(Buzz,800);
      delay(20);
      tone(Buzz,850);
      delay(20);
      tone(Buzz,900);
      delay(20);
      tone(Buzz,950);
      delay(200);

      tone(Buzz,100);
      delay(30);
      tone(Buzz,200);
      delay(30);
      tone(Buzz,400);
      delay(30);
      tone(Buzz,500);
      delay(30);
      tone(Buzz,600);
      delay(30);
      tone(Buzz,700);
      delay(30);
      tone(Buzz,800);
      delay(30);
      tone(Buzz,900);
      delay(300);

      noTone(Buzz);
}
