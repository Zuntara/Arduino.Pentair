#include "Pentair.h"

Pentair pentair(6, 7);

void setup() {
  Serial.begin(115200);

  pentair.SetCallback(PumpChanged);

  pentair.debugLog = true;

  pentair.PumpCommandSetPower(1, true);
/*
  pentair.PumpCommandSetPower(1, false);
  delay(1000);
  pentair.PumpCommandSetPower(1, true);
  delay(1000);
  pentair.PumpStatusCheck(1);
  delay(1000);
*/
analogWrite(3, 255);
}

void loop() {
  analogWrite(5, 255);
  // Inspect the bus on incomming messages
  pentair.ProcessIncommingSerialMessages();

// pentair.PumpCommandSetPower(1, false);
  // Ask pump status
  //pentair.PumpStatusCheck(1);
  analogWrite(5, 0);
  delay(500);
}

void PumpChanged(Pump myPump) {
  analogWrite(2, 255);
  Serial.print("Pump ");
  Serial.print(myPump.pump);
  Serial.println(" has been changed.");
  delay(500);
  analogWrite(2, 0);
}
