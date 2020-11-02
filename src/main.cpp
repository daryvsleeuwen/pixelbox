#include "FastLED.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN 7
//#define CLK_PIN   4
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS 49
#define ROW_LENGTH 7
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 255
#define FRAMES_PER_SECOND 120

int genHue = 0;
bool even = false;
int lastusedpixel = rand();

void setup()
{
  delay(500);
  Serial.begin(38400);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.println(lastusedpixel);
}

int pickRandomPixel()
{
  int pixel = rand() % NUM_LEDS;

  if (pixel == lastusedpixel)
  {
    pickRandomPixel();
  }

  return pixel;
}

void loop()
{
  FastLED.delay(1000 / FRAMES_PER_SECOND);
  FastLED.clear();

  int r = rand() % 255;
  int g = rand() % 255;
  int b = rand() % 255;

  leds[pickRandomPixel()] = CRGB(r, g, b);
  FastLED.show();
}