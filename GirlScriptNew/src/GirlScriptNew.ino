#include <neopixel.h>
#include <adafruit-drv2605-photon.h>
#include "Particle.h"
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

SYSTEM_MODE(AUTOMATIC);

#define PIXEL_PIN D2
#define PIXEL_COUNT 6
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
uint32_t Shake = 0;
uint8_t Gladit = 0;
bool turnOn = true;
bool angry = false;
//-----------------
//pins
int Gerkon = A0;
int Speaker = D4;
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
//--------------------------------------------
//-------------------------------------------активация пинов фотона
    pinMode(MainV,OUTPUT);
    pinMode(Speaker,OUTPUT);
    pinMode(PhCell,INPUT);
    pinMode(IR,INPUT);
    pinMode(Gerkon,INPUT);
    digitalWrite(MainV,0);
    digitalWrite(Speaker,0);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  delay(7000);
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
    }                    //блок включения
//*/

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
switch (stage){

  case 1:         // первый стэйдж

    switch (st1){

      case 1:         //запуск мод
        LedColor(0);
        st1=2;
      break;

      case 2:    //крутит глазами
        LedColor(1);
      break;

      case 3:         // таращение
        LedColor(2);
        st1=0;
      break;
  }

//---------------------------------------------- чувствует контакт с парнем через геркон

  if(digitalRead(Gerkon)==1){
    stage = 2;
    st2=1;
    angry=false;
    Shake=0;
  }
//----------------------------------------------тряска для разозления
//
    if(angry){
      sensors_event_t evt;
      Acc.getEvent(&evt);
      if(abs(evt.acceleration.x)>1 || abs(evt.acceleration.y)>1){
        Shake++;
      }
//-------------------------------------------- если счётчик тряски доходит до 10 переходит в стэйдж 3 начинает злиться
    if(Shake>17){
      stage=3;
      Shake=0;
    }
    delay(300);
    }
  break;

//><>><><><><><><><><>><><><><><><><
  case 2:     // стэйдж 2

      switch (st2){

        case 1:
          LedColor(2);
        break;

        case 2:       //начинает говорить
          digitalWrite(Speaker,1);
          LedColor(2);
          tone(Buzz,random(7,9)*100);
          delay(1000);
          digitalWrite(Speaker,0);
          st2=3;
        break;

        case 3:      // девочка говорит
          LedColor(2);
          tone(Buzz,random(7,9)*100);
          delay(random(1,3)*10);
          LedColor(6);
          noTone(Buzz);
          delay(random(1,2)*5);
        break;

        case 4:      //парень начинает говорить
          LedColor(6);
          st2=5;
        break;

          case 5:     //девочка ждёт перед тем как начать самой говорить
          delay(random(4,7)*1000);
          st2=2;
        break;
        }

        if(analogRead(PhCell)>3500){
          st2=4;
        }

        if(digitalRead(Gerkon)==0){
          LedColor(6);
          stage=1;
          st1=3;
          angry=true;
          noTone(Buzz);
        }


  break;
//><>><><><><><><><><>><><><><><><><
  case 3:

    LedColor(5);        // глаза горят
    tone(Buzz,random(8,10)*100 /* выше*/);
    drv.setWaveform(0, 52);
    drv.setWaveform(1, 0);
    drv.go();
    delay(random(7,10)*100);

    if(digitalRead(IR)==1){
      Gladit++;
//    Serial.println(Gladit);
    }

    LedColor(6);   // глаза не горят
    noTone(Buzz);
    delay(random(1,5)*100);

    if(digitalRead(IR)==1){
      Gladit++;
    }

    if(Gladit>14){
      stage=1;
      st1=3;
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

  case 0:         // проснулась
    for(uint8_t i =0; i<6;i++){

        strip.setPixelColor(1,strip.Color(0,127,127));
        strip.setPixelColor(4,strip.Color(0,127,127));
        strip.show();
        delay(400);
        strip.clear();
        delay(100);
        strip.show();
        delay(300);
      }
    break;

  case 1:       // крутит глазами

      for(uint8_t g = 0; g<3;g++){
        strip.clear();
        strip.show();
        delay(150);
        strip.setPixelColor(g,strip.Color(0,127,127));
        strip.setPixelColor(5-g,strip.Color(0,127,127));
        strip.show();
        delay(300);
      }
  break;

    case 2:   // просто смотрит розовым
      strip.clear();
      strip.show();

      for(uint8_t g = 0; g<6;g++){
        strip.setPixelColor(g,strip.Color(0,127,127,0));
        }
      delay(100);
      strip.show();
    break;

  case 5:         //злится красным
    for(uint8_t g = 1; g<5;g++){
        strip.setPixelColor(g,strip.Color(0,255,0));
      }
      strip.show();
      delay(200);
  break;

  case 6:         //дополнение чтобы стирать цвет между переключениями
      strip.clear();
      delay(100);
      strip.show();
  break;

}
}
