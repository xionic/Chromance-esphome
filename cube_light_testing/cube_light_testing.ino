#include "FastLED.h"

#define SEGMENTS

#define STR_1_LEN 238
#define STR_2_LEN 322

CRGB leds1[STR_1_LEN];
CRGB leds2[STR_2_LEN];

void setup(){

  Serial.begin(115200); // Set the baud rate to 9600
  
 
  for (int led_num = led_num; led_num < STR_1_LEN; led_num++){
    leds1[led_num] = CRGB::Black;
  }

  for (int led_num = led_num; led_num < STR_2_LEN; led_num++){
    leds2[led_num] = CRGB::Black;
  }

  
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500); // Set Current Limit
  FastLED.addLeds<NEOPIXEL, 4>(leds1, STR_1_LEN); 
  FastLED.addLeds<NEOPIXEL, 13>(leds2, STR_2_LEN); 
  FastLED.clear();
  
}

void clear_leds(){
  for (int i = 0; i < STR_1_LEN; i++){
    leds1[i] = CRGB::Black;
  }
  for (int i = 0; i < STR_2_LEN; i++){
    leds2[i] = CRGB::Black;
  }
}

void loop(){
//turn on all
if(1){
  for (int i = 0; i < STR_1_LEN; i++){
    leds1[i] = CRGB::Blue;
    
  }
  for (int i = 0; i < STR_2_LEN; i++){
    leds2[i] = CRGB::Blue;
    
  }
  FastLED.show();
  return;
}

#ifndef SEGMENTS
  for (int i = 0; i < STR_1_LEN; i++){
    leds1[i] = CRGB::Blue;
    FastLED.show();
    delay(10);
    clear_leds();
  }
  for (int i = 0; i < STR_2_LEN; i++){
    leds2[i] = CRGB::Blue;
    FastLED.show();
    delay(10);
    clear_leds();
  }
  
#else
  for (int seg_num = 0; seg_num < STR_1_LEN/14; seg_num++){
    for(int led_num = 0; led_num < 14; led_num++){
      int full_num = 14 * seg_num + led_num;
      Serial.println("Displaying strip: 1 seg: " + String(seg_num) + " led: " + String(led_num) + " full: " + String(full_num));
      leds1[full_num] = CRGB::Blue;
    }
    FastLED.show();
    delay(500);
    clear_leds();
  }

  for (int seg_num = 0; seg_num < STR_2_LEN/14; seg_num++){
    for(int led_num = 0; led_num < 14; led_num++){
      int full_num = 14 * seg_num + led_num;
      Serial.println("Displaying strip: 2 seg: " + String(seg_num) + " led: " + String(led_num) + " full: " + String(full_num));
      leds2[full_num] = CRGB::Blue;
    }
    FastLED.show();
    delay(500);
    clear_leds();
  }
#endif

}
