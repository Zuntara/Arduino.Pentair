#include "BL121.h"
#include "Timer.h"

// forward declare:
void HannaStatusChange(BL121Status status);

// Create our Hanna BL121 instance
BL121 hanna(HannaStatusChange);
Timer checkCycle;

void setup() {
	// Initialize some debug output
	Serial.begin(115200);
	
	checkCycle.every(5000, CheckPoolState);
}

void loop() {
	checkCycle.update();	// Update timer
}

void CheckPoolState(){
	hanna.Update();		// check values and notify when needed
}


void HannaStatusChange(BL121Status status){
	Serial.print("Status change. Has alarm: "); Serial.println(status.HasAlarm);
	
	// Trigger an event here to notify a third party
	
}
