

#include <ESP32Lumie.h>
#include "thingProperties.h"
#include <Button2.h>

#define BUTTON_SUNSET_PIN 13
#define BUTTON_SUNRISE_PIN 12

Button2 buttonSunset;
Button2 buttonSunrise;

void initButtons();

void click(Button2 &btn);

void click(Button2 &btn)
{

  switch (btn.getID())
  {
  case SUNRISE:
    StartSunrise();
    break;
  case SUNSET:
    StartSunset();
    break;

  default:
    break;
  }
}

void initButtons()
{
  auto f_buttonSunrise = []() -> byte
  { EVERY_N_MILLIS(500) { Serial.println(touchRead(BUTTON_SUNRISE_PIN));}
    return touchRead(BUTTON_SUNRISE_PIN) < buttonSunrise.getDebounceTime() ? LOW : HIGH; };
  buttonSunrise.setID(SUNRISE);
  buttonSunrise.setDebounceTime(40);
  buttonSunrise.setButtonStateFunction(f_buttonSunrise);
  buttonSunrise.setPressedHandler(click);
  buttonSunrise.begin(VIRTUAL_PIN);

  auto f_buttonSunset = []() -> byte
  { 
    EVERY_N_MILLIS(500) { Serial.println(touchRead(BUTTON_SUNSET_PIN));}
    return touchRead(BUTTON_SUNSET_PIN) < buttonSunset.getDebounceTime() ? LOW : HIGH; };
  buttonSunset.setID(SUNSET);
  buttonSunset.setDebounceTime(40);
  buttonSunset.setButtonStateFunction(f_buttonSunset);
  buttonSunset.setPressedHandler(click);
  buttonSunset.begin(VIRTUAL_PIN);
}

void setup()
{
  Serial.begin(9600);
  delay(1500);

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(4);
  ArduinoCloud.printDebugInfo();

  LumieSetup();
  initButtons();
}

void loop()
{
  ArduinoCloud.update();
  auto res =   Lumie();
  EVERY_N_SECONDS(5){
    light.setBrightness(res.color.v);
    light.setHue(res.color.h);
    light.setSaturation(res.color.s);
    light.setSwitch(res.state == IDLE ? false : true);  
    status = res.state;
  }
  buttonSunrise.loop();
  buttonSunset.loop();
  
}

void onWakeUpChange(){}

void onLightChange(){}
void onStatusChange(){
  if(status == SUNRISE){
    StartSunrise();
  }
  else if(status == SUNSET){
    StartSunset();
  } else if(status == IDLE){
    LumieStop();
  }
}

void onSunriseDurationChange(){
  //sunriseDuration = sunriseDuration * 60;
}
