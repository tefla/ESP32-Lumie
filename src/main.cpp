#include <Arduino.h>
#include <FastLED.h>
#include <Button2.h>

#define NUM_LEDS 70 // Number of LEDS in the strip
#define DATA_PIN 16 // Data pin that LED strip is attached to
#define BUTTON_SUNSET_PIN 15
#define BUTTON_SUNRISE_PIN 4

CRGB leds[NUM_LEDS];
int ledOrder[NUM_LEDS];

int brightness = 0;
int sunriseDuration = 1 * 20; // Duration of sunrise in seconds
int sunsetDuration = 1 * 20; // Duration of sunset in seconds
long interval;
unsigned long previousMillis = 0;
int currentLedIndex = 0;

Button2 buttonSunset;
Button2 buttonSunrise;

enum State
{
  IDLE,
  SUNRISE,
  SUNSET
};

State state = IDLE;

void shuffleLEDOrder();
void sunrise();
void sunset();
void debugPrint();
void click(Button2 &b);
byte capStateHandler();


void click(Button2 &btn)
{

  switch (btn.getID())
  {
  case SUNRISE:
    brightness = 0;
    state = SUNRISE;
    interval = (sunriseDuration * 1000) / 255;
    FastLED.setBrightness(254);
    Serial.println("click Sunrises");
    break;
  case SUNSET:
    brightness = 254;
    state = SUNSET;
    interval = (sunsetDuration * 1000) / 255;
    Serial.println("click Sunset");
    break;

  default:
    break;
  }
}

void initButtons()
{
  auto f_buttonSunrise = []() -> byte
  { return touchRead(BUTTON_SUNRISE_PIN) < buttonSunrise.getDebounceTime() ? LOW : HIGH; };
  buttonSunrise.setID(SUNRISE);
  buttonSunrise.setDebounceTime(35);
  buttonSunrise.setButtonStateFunction(f_buttonSunrise);
  buttonSunrise.setPressedHandler(click);
  buttonSunrise.begin(VIRTUAL_PIN);

  auto f_buttonSunset = []() -> byte
  { 
    EVERY_N_MILLIS(500) { Serial.println(touchRead(BUTTON_SUNSET_PIN));}
    return touchRead(BUTTON_SUNSET_PIN) < buttonSunset.getDebounceTime() ? LOW : HIGH; };
  buttonSunset.setID(SUNSET);
  buttonSunset.setDebounceTime(35);
  buttonSunset.setButtonStateFunction(f_buttonSunset);
  buttonSunset.setPressedHandler(click);
  buttonSunset.begin(VIRTUAL_PIN);
}

void setup()
{
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  shuffleLEDOrder();
  initButtons();
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval / NUM_LEDS)
  {
    // debugPrint();
    previousMillis = currentMillis;
    if (state == SUNRISE)
    {
      sunrise();
    }
    else if (state == SUNSET)
    {
      sunset();
    }
    FastLED.show();
  }

  buttonSunrise.loop();
  buttonSunset.loop();
}

void shuffleLEDOrder()
{
  // Create a shuffled list of LED indexes
  for (int i = 0; i < NUM_LEDS; i++)
  {
    ledOrder[i] = i;
  }

  // Fisher-Yates shuffle algorithm
  for (int i = NUM_LEDS - 1; i > 0; i--)
  {
    int j = random(i + 1);
    int temp = ledOrder[i];
    ledOrder[i] = ledOrder[j];
    ledOrder[j] = temp;
  }
}

void sunrise()
{
  if (brightness < 255)
  {
    int i = ledOrder[currentLedIndex];
    int hue = map(brightness, 0, 255, 0, 40);  // map brightness to hue for sunrise effect (0 = red, 96 = yellow)
    int sat = map(brightness, 0, 255, 255, 150); //
    leds[i] = CHSV(hue, sat, brightness);      // set the brightness in the value field of the HSV color

    currentLedIndex++;
    if (currentLedIndex >= NUM_LEDS)
    { // if we've updated all LEDs, increase brightness and reset index
      brightness++;
      currentLedIndex = 0;
    }
  }
  else
  {
    state = IDLE; // reset the state when the sunrise is complete
    debugPrint();
  }
}

void sunset()
{
  if (brightness > 0)
  {
    int i = ledOrder[currentLedIndex];
    int hue = map(brightness, 0, 255, 0, 40);  // map brightness to hue for sunrise effect (0 = red, 96 = yellow)
    int sat = map(brightness, 0, 255, 255, 150); //
    leds[i] = CHSV(hue, sat, brightness);      // set the brightness in the value field of the HSV color

    currentLedIndex++;
    if (currentLedIndex >= NUM_LEDS)
    { // if we've updated all LEDs, decrease brightness and reset index
      brightness--;
      currentLedIndex = 0;
    }
  }
  else
  {
    state = IDLE; // reset the state when the sunset is complete
    FastLED.setBrightness(0);
    debugPrint();
  }
}

void debugPrint()
{
  Serial.println("----- Debug Information -----");
  Serial.print("Current state: ");
  switch (state)
  {
  case IDLE:
    Serial.println("IDLE");
    break;
  case SUNRISE:
    Serial.println("SUNRISE");
    break;
  case SUNSET:
    Serial.println("SUNSET");
    break;
  }
  Serial.print("Current brightness: ");
  Serial.println(brightness);
  Serial.print("Interval (ms): ");
  Serial.println(interval);
  Serial.println("-----------------------------");
}
