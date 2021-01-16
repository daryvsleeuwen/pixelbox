# Interactive Pixelbox

An interactive Pixelbox that uses LED lights to create animations based on user input

## Pixelbox animations fases

The Pixelbox program provides 3 animation fases of which 2 and 3 are interactive

[Fase 3 demo](https://www.youtube.com/watch?v=C89gsYPGuIQ&feature=youtu.be)

&nbsp;

## Hardware usage

#### Microcontroller
For the Pixelbox to work you need a microcontroller that can work with 5V. For this project the [Arduino Mega](https://store.arduino.cc/arduino-mega-2560-rev3) is used

#### LED strip
You will need a LED strip that has the ability to address each individual LED. For this project the [WS2811B](https://www.amazon.nl/WS2812B-Enkele-adresseerbare-niet-waterdicht-printplaat/dp/B01MZGXY21/ref=asc_df_B01MZGXY21/?tag=nlshogostdde-21&linkCode=df0&hvadid=430557236523&hvpos=&hvnetw=g&hvrand=3007921579706907420&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=1010485&hvtargid=pla-311046353963&psc=1) LED strip is used. That LED strip works on 5V. You also have 12V version, but note that your power supply (Arduino with 5V in my case) also needs to provide 12V

#### Ultrasonic distance scanner
To make the Pixelbox interactive with user input, a [Ultrasonic sensor](https://howtomechatronics.com/tutorials/arduino/ultrasonic-sensor-hc-sr04/) is used. Based on how far away an object such as your hand is placed, the animation in a specific fase will react on that input

&nbsp;
[Fase 2 demo](https://www.youtube.com/watch?v=GRwhG71XTbM&feature=youtu.be)

&nbsp;

## Code setup
Before the code will work properly in your situation you will need to change a few basic things. Here are the settings for your Pixelbox. Change them in the **main.cpp** file according to your situation.

````
#define DATA_PIN YOUR_DATA_PIN      //Microcontroller pin for the LED strip data line
#define LED_TYPE WS2812             //Your LED strip type. Check https://github.com/FastLED/FastLED for the supported LED strips
#define COLOR_ORDER GRB             //Color order of the animation. GRB by default
#define NUM_LEDS YOUR_AMOUTN        //Number of LEDs in your strip (COLUMN_LENGTH * ROW_LENGTH in this case)
#define COLUMN_LENGTH YOUR_LENGTH   //Number of LEDs on the Y axis of your Pixelbox
#define ROW_LENGTH YOUR_LENGTH      //Number of LEDs on the X axis of your Pixelbox
#define BRIGHTNESS 255              //General brightness of the LEDs
#define FRAMES_PER_SECOND 30        //Animations speed. 60 fps will be two times faster than 30 fps
````
