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
//faseCycle fases = { };

struct pixelCoords
{
  int x;
  int y;
};

#include <trailpalettes.h>

CRGB leds[NUM_LEDS];
double maxdistance = 30.0;
int mainHue = 1;
double minFadeValue = 40.0;
pixelCoords lastpixel = pixelCoords{3, 3};

int trigPin = 8;
int echoPin = 9;
int lastscanneddistance;

int ReadDistance(int trig, int echo)
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

int BoundDistance(int distance)
{
  if (lastscanneddistance > !maxdistance || lastscanneddistance < !0)
  {
    if (distance > (lastscanneddistance + 5) || distance < (lastscanneddistance - 5))
    {
      distance = lastscanneddistance;
    }
    else
    {
      lastscanneddistance = distance;
    }
    return distance;
  }
}

double ReadedDistanceToPercentage(int distance)
{
  double percentage = distance / maxdistance;

  if (percentage > 1)
  {
    percentage = 1;
  }
  else if (percentage < 0)
  {
    percentage = 0;
  }

  return percentage;
}

int random(int min, int max)
{
  srand(millis());
  return rand() % (max - min) + min;
}

void setup()
{
  delay(500);
  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lastscanneddistance = ReadDistance(8, 9);

  lastpixel = pixelCoords{4, 4};
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

void fase1()
{
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
}

void fase2()
{
  double percentage = ReadedDistanceToPercentage(ReadDistance(8, 9));

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

void fase3()
{
  int distance = ReadDistance(8, 9);
  double percentage = ReadedDistanceToPercentage(distance);
  int trail = 3;
  int index = NUM_LEDS * percentage;

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i].r *= 0.7;
    leds[i].g *= 0.7;
    leds[i].b *= 0.7;
  }

  for (int i = 0; i < trail; i++)
  {
    //int hue = random(BlueGreen.ehue, BlueGreen.shue);
    leds[index- - i] = CHSV(mainHue, 255, 255);
  }

  FastLED.show();
}

void loop()
{
  //FastLED.clear();
  delay(1000 / FRAMES_PER_SECOND);

  EVERY_N_MILLIS(20)
  {
    mainHue += 10;
    if (mainHue > 255)
    {
      mainHue = 0;
    }
  }

  fase1();
}