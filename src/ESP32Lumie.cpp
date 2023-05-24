#include "ESP32Lumie.h"

#define NUM_LEDS 70 // Number of LEDS in the strip
#define DATA_PIN 16 // Data pin that LED strip is attached to

State state = IDLE;

CRGB leds[NUM_LEDS];
int ledOrder[NUM_LEDS];

int brightness = 0;
int sunriseDuration = 1 * 20; // Duration of sunrise in seconds
int sunsetDuration = 1 * 20;  // Duration of sunset in seconds
long interval;
unsigned long previousMillis = 0;
int currentLedIndex = 0;

ESP32LumieResult Lumie()
{
    unsigned long currentMillis = millis();
    ESP32LumieResult result;
    result.state = state;

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
    auto led = leds[ledOrder[currentLedIndex]];

    result.color = rgb2hsv_approximate(led);
    return result;
}

void LumieSetup()
{
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setDither(DISABLE_DITHER);
    shuffleLEDOrder();
}
void LumieStop() 
{
    state = IDLE;
}
void StartSunrise()
{
    brightness = 0;
    state = SUNRISE;
    interval = (sunriseDuration * 1000) / 255;
    FastLED.setBrightness(254);
    Serial.println("click Sunrises");
}
void StartSunset()
{
    brightness = 254;
    state = SUNSET;
    interval = (sunsetDuration * 1000) / 255;
    Serial.println("click Sunset");
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
        int hue = map(brightness, 0, 255, 0, 40);    // map brightness to hue for sunrise effect (0 = red, 96 = yellow)
        int sat = map(brightness, 0, 255, 255, 150); //
        leds[i] = CHSV(hue, sat, brightness);        // set the brightness in the value field of the HSV color

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
        int hue = map(brightness, 0, 255, 0, 40);    // map brightness to hue for sunrise effect (0 = red, 96 = yellow)
        int sat = map(brightness, 0, 255, 255, 150); //
        leds[i] = CHSV(hue, sat, brightness);        // set the brightness in the value field of the HSV color

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
