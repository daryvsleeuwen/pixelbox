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
#define FRAMES_PER_SECOND 120

struct pixelCoords
{
  int x;
  int y;
};

CRGB leds[NUM_LEDS];
int mainHue = 1;
int minFadeValue = 10;
const int traillength = 20;
int fadestep = (255 - minFadeValue) / (traillength - 1);
struct pixelCoords trail[traillength];

void setup()
{
  delay(500);
  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  for (int i = 0; i < traillength; i++)
  {
    trail[i] = pixelCoords{4, 4};
  }
}

int convertPixelCoords(struct pixelCoords coords)
{
  return (coords.x - 1) + ((coords.y - 1) * ROW_LENGTH);
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

void shiftPixelTrail(struct pixelCoords pixeltrail[])
{
  struct pixelCoords temp[traillength];

  //COPY TRAIL ARRAY TO TEMP ARRAY
  for (int i = 0; i < traillength; i++)
  {
    temp[i] = pixeltrail[i];
  }

  //MODIFY ORIGINAL TRAIL ARRAY
  for (int i = 1; i < traillength + 1; i++)
  {
    pixeltrail[i] = temp[i - 1];
  }
}

void loop()
{
  FastLED.clear();

   EVERY_N_MILLIS(20)
  {
    mainHue += 10;
    if (mainHue > 255)
    {
      mainHue = 0;
    }
  }

  struct pixelCoords npixel;
  npixel = findPixelNeighbour(trail[0]);
  if (npixel.x == trail[0].x && npixel.y == trail[0].y)
  {
    npixel = findPixelNeighbour(trail[0]);
  }

  trail[0] = npixel;
  shiftPixelTrail(trail);

  for (int i = 0; i < traillength; i++)
  {
    int index = convertPixelCoords(trail[i]);
    leds[index] = CHSV(mainHue, 255, fadestep * i);
  }

  FastLED.show();
  delay(30);
}