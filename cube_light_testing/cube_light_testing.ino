#include "FastLED.h"

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
  FastLED.addLeds<NEOPIXEL, 4>(leds1, 280); 
  FastLED.addLeds<NEOPIXEL, 13>(leds2, 280); 
  FastLED.clear();
  
}

void loop(){ 
  for (int seg_num = 0; seg_num < STR_1_LEN/14; seg_num++){
    for(int led_num = 0; led_num < 14; led_num++){
      int full_num = 14 * seg_num + led_num;
      Serial.println("Displaying strip: 1 seg: " + String(seg_num) + " led: " + String(led_num) + " full: " + String(full_num));
      leds1[led_num] = CRGB::Blue;
    }
    FastLED.show();
    delay(100);
    FastLED.clear();
  }

  for (int seg_num = 0; seg_num < STR_2_LEN/14; seg_num++){
    for(int led_num = 0; led_num < 14; led_num++){
      int full_num = 14 * seg_num + led_num;
      Serial.println("Displaying strip: 2 seg: " + String(seg_num) + " led: " + String(led_num) + " full: " + String(full_num));
      leds2[led_num] = CRGB::Blue;
    }
    FastLED.show();
    delay(100);
    FastLED.clear();
  }


}
