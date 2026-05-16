#include <FastLED.h>

#define NUM_LEDS 8
CRGB leds[NUM_LEDS];

#define LED_PIN_P1 5
#define LED_PIN_P2 19
#define LED_PIN_P3 13
#define LED_PIN_P4 14

void setup() {
  Serial.begin(115200);
  
  FastLED.addLeds<WS2812B, LED_PIN_P1, RGB>(leds, 0, 2).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, LED_PIN_P2, RGB>(leds, 2, 2).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, LED_PIN_P3, RGB>(leds, 4, 2).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, LED_PIN_P4, RGB>(leds, 6, 2).setCorrection(TypicalLEDStrip);
  
  FastLED.setBrightness(255);
  Serial.println("FastLED Test Started");
}

void loop() {
  // Turn all Red
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
  Serial.println("Color: RED");
  delay(1000);
  
  // Turn all Green
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  FastLED.show();
  Serial.println("Color: GREEN");
  delay(1000);
  
  // Turn all Blue
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
  FastLED.show();
  Serial.println("Color: BLUE");
  delay(1000);
  
  // Turn all White
  fill_solid(leds, NUM_LEDS, CRGB::White);
  FastLED.show();
  Serial.println("Color: WHITE");
  delay(1000);
  
  // Turn all Off
  FastLED.clear();
  FastLED.show();
  Serial.println("Color: OFF");
  delay(1000);
}
