#include "arduino_secrets.h"
#include <Arduino.h>
#include <esp_wifi.h>
#include <ArduinoOTA.h>
#include <WiFi.h>

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char DEVICE_LOGIN_NAME[]  = "e1bd483b-c316-4f1f-b009-d20fe93d1143";

const char SSID[]               = SECRET_SSID;    // Network SSID (name)
const char PASS[]               = SECRET_OPTIONAL_PASS;    // Network password (use for WPA, or use as key for WEP)
const char DEVICE_KEY[]  = SECRET_DEVICE_KEY;    // Secret device password

void onLightChange();
void onWakeUpChange();
void onStatusChange();
void onSunsetLengthChange();

CloudColoredLight light;
CloudSchedule wakeUp;
CloudInt status;
CloudTime sunsetLength;

void initProperties(){

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(light, READWRITE, ON_CHANGE, onLightChange);
  ArduinoCloud.addProperty(wakeUp, READWRITE, ON_CHANGE, onWakeUpChange);
  ArduinoCloud.addProperty(status, READWRITE, ON_CHANGE, onStatusChange);
  ArduinoCloud.addProperty(sunsetLength, READWRITE, ON_CHANGE, onSunsetLengthChange);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
