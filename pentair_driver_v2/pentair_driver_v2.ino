#include "Pentair.h"

// Create our pentair instance
Pentair pentair(6, 7);		// RX, TX pins for RS-485 (shield)

void setup() {
	// Initialize some debug output
	Serial.begin(115200);
	
	// set a callback method for when a pump status changes
	pentair.SetCallback(PumpChanged);
	// indicate to do some debug logging on the serial bus
	pentair.debugLog = true;

	// Tell the pump to startup
	pentair.PumpCommandSetPower(1, true);
	
	// Just tell a locally attached LED at pin A3 to light up
	analogWrite(3, 255);
}

void loop() {
	// Light up a LED on pin A5
	analogWrite(5, 255);
	
	// Inspect the bus for incomming messages and process them
	pentair.ProcessIncommingSerialMessages();

	// Dim the led at pin A5
	analogWrite(5, 0);
	
	// Wait a little bit
	delay(500);
}

// Gets called when there is a change in attributes on the given pump
void PumpChanged(Pump myPump) {
	// Light up a LED on pin A2
	analogWrite(2, 255);
	
	Serial.print("Pump "); Serial.print(myPump.pump); Serial.println(" has been changed.");
	
	delay(500);
	
	// Dim LED at pin A2
	analogWrite(2, 0);
}
