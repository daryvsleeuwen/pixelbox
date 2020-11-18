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

typedef void (*faseCycle[])();
// faseCycle fases = { test };

struct pixelCoords
{
  int x;
  int y;
};

CRGB leds[NUM_LEDS];
int mainHue = 1;
double maxdistance = 30.0;
int faseduration = 3000;
int minFadeValue = 40.0;
const int traillength = 40;
int fadestep = (255 - minFadeValue) / (traillength - 1);
struct pixelCoords trail[traillength];

int trigPin = 8;
int echoPin = 9;
int lastscanneddistance;
int stripethickness = 1;
struct pixelCoords stripe[ROW_LENGTH];

int ReadDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  if (distance)
    return distance;
}

void setup()
{
  delay(500);
  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  for (int i = 0; i < traillength; i++)
  {
    trail[i] = pixelCoords{4, 4};
  }
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

void fase1()
{
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
}

void fase2()
{
  int distance = ReadDistance();
  double percentage = distance / maxdistance;

  if (percentage > 1)
  {
    percentage = 1;
  }
  else if (percentage < 0)
  {
    percentage = 0;
  }

  int y = ROW_LENGTH * percentage;
  int highersteps = ROW_LENGTH - y;
  int lowerbase = 100 / y;
  int higherbase = 100 / highersteps;

  for (int i = 0; i < y; i++)
  {
    for (int x = 0; x < COLUMN_LENGTH; x++)
    {
      int index = convertPixelCoords(pixelCoords{x, i});
      leds[index] = CHSV(mainHue, 255, lowerbase * i + minFadeValue);
    }
  }

  for (int j = 0; j < highersteps; j++)
  {
    for (int x = 0; x < COLUMN_LENGTH; x++)
    {
      int index = convertPixelCoords(pixelCoords{x, y + j});
      leds[index] = CHSV(mainHue, 255, 100 - (higherbase * j));
    }
  }

  for (int x = 0; x < COLUMN_LENGTH; x++)
  {
    int index = convertPixelCoords(pixelCoords{x, y});
    leds[index] = CHSV(mainHue, 255, 255);
  }

  FastLED.show();
}

void loop()
{
  FastLED.clear();
  delay(1000 / FRAMES_PER_SECOND);

  EVERY_N_MILLIS(20)
  {
    mainHue += 10;
    if (mainHue > 255)
    {
      mainHue = 0;
    }
  }

  fase2();
}