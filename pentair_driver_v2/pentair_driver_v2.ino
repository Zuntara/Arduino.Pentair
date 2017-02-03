#include "Pentair.h"
#include "SoftwareSerial.h"

// Create our pentair instance
Pentair pentair(6, 7);		// RX, TX pins for RS-485 (shield)

void setup() {
  analogWrite(3, 0);
	// Initialize some debug output
	Serial.begin(115200);
	Serial.println("Initializing...");

	// set a callback method for when a pump status changes
	pentair.SetCallback(PumpChanged);
	// indicate to do some debug logging on the serial bus
	pentair.debugLog = true;

  delay(500);
  analogWrite(3, 50);
	// Tell the pump to startup
	pentair.PumpCommandSetPower(1, false);
  analogWrite(3, 100);
	delay(1000);
  analogWrite(3, 150);
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
analogWrite(4, 255);
  pentair.PumpStatusCheck(1);
analogWrite(4, 0);
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

// Full Cyclic cycle
// C: A500 d=60 s=10 c=04 l=01 FF       <0219> SETCTRL remote
// P: A500 d=10 s=60 c=04 l=01 FF       <0219> CTRL is remote
// C: A500 d=60 s=10 c=01 l=04 02E40012 <0212> WRITE (18) to 0x02e4
// P: A500 d=10 s=60 c=01 l=02 0012 <012A>     VALIS (18)
// C: A500 d=60 s=10 c=05 l=01 06       <0121> SETMOD 06 (Feature 1)
// P: A500 d=10 s=60 c=05 l=01 06       <0121> MOD is 06
// C: A500 d=60 s=10 c=06 l=01 0A       <0126> SETRUN 0a Started
// P: A500 d=10 s=60 c=06 l=01 0A       <0126> RUN is 0a Started
// C: A500 d=60 s=10 c=07 l=00          <011C> SEND status
// P: A500 d=10 s=60 c=07 l=0f 0A0602024908B1120000000A000F22 <028E>

// If the controller releases the pump the cyclic sequence changes to:

// C: A500 d=60 s=10 c=04 l=01 00 <011A> SETCTRL local
// P: A500 d=10 s=60 c=04 l=01 00 <011A> CTRL is local
/*
void DoCycleTest(){
	pentair.PumpStatusCheck(1); // set to remote, ask status of pump 1, set back to local
}
*/
