#include <Button2.h>
#include <FastLED.h>

#define NUM_LEDS 70 // Number of LEDS in the strip
#define DATA_PIN 16 // Data pin that LED strip is attached to

enum State
{
  IDLE,
  SUNRISE,
  SUNSET
};

struct ESP32LumieResult
{
  CHSV color;
  State state;
};



void shuffleLEDOrder();
void sunrise();
void sunset();
void debugPrint();
void click(Button2 &b);

void StartSunrise();
void StartSunset();

ESP32LumieResult Lumie();
void LumieSetup();

void LumieStop();