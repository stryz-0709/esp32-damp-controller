/*
 * helper.cpp
 *
 *  Created on: Aug 26, 2024
 *  Last modified: Aug 26, 2024
 *  Version 1: Init
 *  Version 2: Add TOWER_LED Pin
 *  Author: Minh Tri
 */

//HELPER FUNCTIONS//

#include "print.h"

///Pin setup///
void esp1PinSetup() {
  for (int i = 0; i < 2; i++) pinMode(SWITCHES[i], INPUT_PULLDOWN);
  for (int i = 0; i < 3; i++) pinMode(BUTTONS[i], INPUT_PULLUP);
  for (int i = 0; i < 7; i++) pinMode(OUT_PINS[i], INPUT_PULLDOWN);
  for (int i = 0; i < 4; i++){
    pinMode(ESP1_OUTPUTS[i], OUTPUT);

    //TURN OFF ALL RELAYS (ACTIVE = LOW)
    digitalWrite(ESP1_OUTPUTS[i], HIGH);
  }
}

void esp2PinSetup(){
  for (int i = 0; i < 7; i++) pinMode(IN_PINS[i], INPUT_PULLDOWN);
  for (int i = 0; i < 4; i++) pinMode(ESP2_OUTPUTS[i], OUTPUT);
  //TURN OFF ALL LEDS
  digitalWrite(GET_WATER_LED, LOW);
  digitalWrite(REMOVE_WATER_LED, LOW);

  //RELAYS (ACTIVE = LOW)
  digitalWrite(START_BUTTON, LOW);
  digitalWrite(TOWER_LED, LOW);
}

bool isNumber(String payload) {
  for (int i = 0; i < payload.length(); i++) {
    if (!isDigit(payload[i])) return false;
  }
  return true;
}

//Background water check//
void backgroundCheck(){
  outToIn = (outLevel - inLevel > h1);
  inToOut = (inLevel - outLevel > h2);
}


//Read input//
bool readInputs(){
  prevEndButton = endButton;
  prevGetWater = getWater;
  prevRemoveWater = removeWater;
  endButton = !digitalRead(END_BUTTON);
  getWater = !digitalRead(GET_WATER);
  removeWater = !digitalRead(REMOVE_WATER);
  top_val = !digitalRead(GATE_TOP);
  bot_val = !digitalRead(GATE_BOTTOM);

  printInputs();

  return (prevGetWater != getWater) || (prevRemoveWater != removeWater);
}

void decodePendingMessage(String pendingMessage){
  if (pendingMessage == "DEBUG" || pendingMessage == "MANUAL"){
    gateStatus = "STOPPED";
    gateMode = pendingMessage;
  } 
  else if (isNumber(pendingMessage)) {
    int value = pendingMessage.toInt();
    if (value >= 30) debugOut = value % 30;
    else if (value >= 20) debugIn = value % 20;
    else if (value >= 10){
      h2 = value % 10;
      updateEeprom(2, h2);
    } 
    else{
      h1 = value;
      updateEeprom(1, h1);
    } 
  }
  else if (pendingMessage == "GET_WATER"){
    if (outToIn) gateStatus = pendingMessage;
    else{
      Serial.println("Condition outToIn not met.");
      errorCode = "H1";
      gateStatus == "STOPPED";
    }
  }
  else if (pendingMessage == "REMOVE_WATER"){
    if (inToOut) gateStatus = pendingMessage;
    else{
      Serial.println("Condition inToOut not met.");
      errorCode = "H2";
      gateStatus == "STOPPED";
    }
  }
  else gateStatus = pendingMessage;
}