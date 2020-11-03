#include "FastLED.h"
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN 7

#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS 49
#define ROW_LENGTH 7
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 255
#define FRAMES_PER_SECOND 120

struct pixelCoords
{
  int x;
  int y;
};

int mainHue = 0;
struct pixelCoords lastpixel;
int currentpixels[NUM_LEDS];
int fadeduration = 1200;

void setup()
{
  delay(500);
  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  srand(millis());
  lastpixel = {4, 4};
}

int convertPixelCoords(struct pixelCoords coords)
{
  return coords.x + (coords.y * ROW_LENGTH);
}

// int pickRandomPixel()
// {
//   int lastusedpixel = random16(NUM_LEDS);
//   return lastusedpixel;
// }

struct pixelCoords findPixelNeighbour(struct pixelCoords coords)
{
  struct pixelCoords temp = coords;

  srand(millis());
  int xAddition = (rand() % 3) - 1;
  int yAddition = (rand() % 3) - 1;

  if (xAddition == 0 && yAddition == 0)
  {
    findPixelNeighbour(coords);
  }

  //OUT OF BORDER CHECK
  if (temp.x + xAddition > ROW_LENGTH - 1 || temp.y + yAddition > ROW_LENGTH - 1 || temp.x + xAddition < 0 || temp.y + yAddition < 0)
  {
    findPixelNeighbour(coords);
  }

  else
  {
    temp.x += xAddition;
    temp.y += yAddition;

    return temp;
  }
}

void loop()
{
  FastLED.clear();

  EVERY_N_MILLIS(20)
  {
    mainHue += 30;
    if (mainHue > 255)
    {
      mainHue = 0;
    }
  }
  struct pixelCoords npixel;
  npixel = findPixelNeighbour(lastpixel);
  if (npixel.x == lastpixel.x && npixel.y == lastpixel.y)
  {
    npixel = findPixelNeighbour(lastpixel);
  }

  int lindex = convertPixelCoords(lastpixel);
  int nindex = convertPixelCoords(npixel);

  leds[nindex] = CHSV(mainHue, 255, 255);
  leds[lindex] = CHSV(mainHue, 255, 140);

  FastLED.show();
  lastpixel = npixel;
  delay(100);
}