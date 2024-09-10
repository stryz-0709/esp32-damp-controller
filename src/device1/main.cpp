
#include "global.h"

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  esp1PinSetup();
  eepromSetup();
  apEspSetup("Cổng 1", 192, 168, 1, 1, esp2, WIFI_AP_STA, true);
  serverSetup(server);
}

void loop() {
  unsigned long currentMillis = millis();
  //// Water level////
  if (currentMillis - prevWaterMillis >= waterInterval){
    prevWaterMillis = currentMillis;
    readWaterLevel(OUT_PINS, outSum, water1Count, outLevel, outMeasured, gateMode, debugOut, true);

  }

  if (inMeasured && outMeasured){
    backgroundCheck();
    printInfo();

    inMeasured = outMeasured = 0;
  }
  /////////////////

  //Button logic////
  if (currentMillis - prevButtonMillis >= buttonInterval) {
    prevButtonMillis = currentMillis;
    buttonLogic();
  }
  ////////////////

  //// Gate logic ////
  if (currentMillis - prevGateMillis >= gateInterval) {
    prevGateMillis = currentMillis;
    gateLogic();

    //Print gate info
    if (gateCount == 5){
      printGateInfo();
      gateCount = 0;
    }
  }
  /////////////////

  //Force closing////
  if (closingPhase){
    if ((currentMillis - prevBotMillis) >= botInterval) {
      forceClose();
    }
  }

  if (motorStatus == "STANDBY"){
    if ((currentMillis - motorStandbyMillis) >= standByInterval) 
      deactivateMotor();
  }
  else if (motorStatus == "TRIGGERED"){
    if ((currentMillis - motorTrigMillis) >= motorTrigInterval) 
      triggerMotor();
  }

  if ((currentMillis - sendMobileMillis) >= sendMobileInterval){
    sendMobileMillis = currentMillis;
    sendToAndroid();
  }
}