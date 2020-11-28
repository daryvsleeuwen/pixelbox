#include "FastLED.h"
#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN 7

#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS 49
#define COLUMN_LENGTH 7
#define ROW_LENGTH 7
#define BRIGHTNESS 255
#define FRAMES_PER_SECOND 30

struct pixelCoords
{
  int x;
  int y;
};

CRGB leds[NUM_LEDS];
int mainHue = 1;
pixelCoords lastpixel = pixelCoords{3, 3};

void setup()
{
  delay(500);
  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

}

int convertPixelCoords(struct pixelCoords coords)
{
  int index = coords.x + (coords.y * ROW_LENGTH);

  if (index > NUM_LEDS)
  {
    return NUM_LEDS;
  }
  else if (index < 0)
  {
    index = 0;
  }
  else
  {
    return index;
  }
}

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
  if (temp.x + xAddition > ROW_LENGTH || temp.y + yAddition > COLUMN_LENGTH || temp.x + xAddition < 1 || temp.y + yAddition < 1)
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
  delay(1000 / FRAMES_PER_SECOND);

   EVERY_N_MILLIS(20)
  {
    mainHue += 10;
    if (mainHue > 255)
    {
      mainHue = 0;
    }
  }

  struct pixelCoords newpixel;
  newpixel = findPixelNeighbour(lastpixel);
  if (newpixel.x == lastpixel.x && newpixel.y == lastpixel.y)
  {
    newpixel = findPixelNeighbour(lastpixel);
  }

  lastpixel = newpixel;
  fadeToBlackBy(leds, NUM_LEDS, 60);

  int index = convertPixelCoords(newpixel);
  leds[index] = CHSV(mainHue, 255, 255);

  FastLED.show();
  delay(20);
}