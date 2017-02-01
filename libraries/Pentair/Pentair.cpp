/*
Pentair.cpp - Library for Pentair IntelliFlo.
Created by Filip Slaets, January 23, 2017.
*/
// For Arduino 1.0 and earlier
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Pentair.h"
#include "SoftwareSerial.h"
#include "../LinkedList/LinkedList.h"



//*******************************************************************************//
// PumpHolder methods
//*******************************************************************************//

PumpHolder::PumpHolder() {
	_currentPumpStatus[1].pump = 1;
	_currentPumpStatus[2].pump = 2;
}
PumpHolder::~PumpHolder() {

}
void PumpHolder::pumpAck(LinkedList<byte> data, int dataLen, int pump, int counter) {
	Serial.println("Responded with ACK");
}
void PumpHolder::setCurrentProgramFromController(int program, int from, LinkedList<byte> data, int counter) {
	int pump = getPumpNumber(data);
	if (_currentPumpStatus[pump].currentprogram != program) {
		_currentPumpStatus[pump].currentprogram = program;
		Serial.print("Set Current Program to ");
		Serial.println(program);
	}
	//container.io.emitToClients('pump')
}
int PumpHolder::getCurrentProgram(int pump) {
	return _currentPumpStatus[pump].currentprogram;
}
void PumpHolder::setCurrentProgram(int pump, int program, int rpm) {
	//console.log('pump: %s,  program %s, rpm %s', pump, program, rpm)
	if (rpm == -1) {
		_currentPumpStatus[pump].currentprogram = program;
	}
	else {
		//string str = "program" + program + "rpm";
		//currentPumpStatus[pump][str] = rpm;
		_currentPumpStatus[pump].currentprogram = program;
	}
	//container.io.emitToClients('pump')
}
void PumpHolder::saveProgramAs(int program, int rpm, int from, LinkedList<byte> data, int counter) {
	int pump = getPumpNumber(data);
	/*if (currentPumpStatus[pump].programXrpm != = rpm) {
	currentPumpStatus[pump].programXrpm = rpm;
	if (s.logPumpMessages)
	logger.verbose('Msg# %s   %s: Save Program %s as %s RPM %s', counter, program, container.constants.ctrlString[from], rpm, JSON.stringify(data));
	}
	container.io.emitToClients('pump')*/
	Serial.println("RPM Saved");
}
void PumpHolder::setRemoteControl(bool remoteControl, int from, LinkedList<byte> data, int counter) {
	int pump = getPumpNumber(data);
	_currentPumpStatus[pump].remotecontrol = remoteControl;
}
void PumpHolder::setRunMode(byte mode, int from, LinkedList<byte> data, int counter) {

}
void PumpHolder::setPowerFromController(bool power, int from, LinkedList<byte> data, int counter) {
	int pump = getPumpNumber(data);
	if (_currentPumpStatus[pump].power != power) {
		_currentPumpStatus[pump].power = power;
	}
}
int PumpHolder::getPumpNumber(LinkedList<byte> data) {
	int pump = -1;
	if (data.get(pump_packetFields_FROM) == PUMP1 || data.get(pump_packetFields_DEST) == PUMP1) {
		pump = 1;
	}
	else {
		pump = 2;
	}
	return pump;
}
void PumpHolder::setDuration(int pump, int duration) {
	_currentPumpStatus[pump].duration = duration;
}
void PumpHolder::setTime(int pump, int hour, int min) {
	//var timeStr = container.helpers.formatTime(hour, min);
	//currentPumpStatus[pump].time = timeStr;
}
void PumpHolder::setPower(int pump, bool power) {
	_currentPumpStatus[pump].power = power;
	if (power == false)
	{
		_currentPumpStatus[pump].duration = 0;
		_currentPumpStatus[pump].currentprogram = 0;
	}
}
void PumpHolder::provideStatus(LinkedList<byte> data, int counter) {
	//if (s.logPumpMessages)
	//	logger.verbose('Msg# %s   %s --> %s: Provide status: %s', counter, c.ctrlString[data[c.packetFields.FROM]], c.ctrlString[data[c.packetFields.DEST]], JSON.stringify(data));
	Serial.println("Msg# Provide status");
}
void PumpHolder::setPumpStatus(int pump, int hour, int min, int run, int mode, int drivestate, int watts, int rpm, int ppc, int err, int timer, LinkedList<byte> data, int counter) {
	Serial.print("Msg# Set Pump Status for pump: ");
	Serial.println(pump);

	setTime(pump, hour, min);
	int needToEmit = 0;
	String whatsDifferent = "";

	if (_currentPumpStatus[pump].watts == -1) {
		Serial.println("Assigning fields...");
		needToEmit = 1;
		Serial.println(run);
		Serial.println(mode);
		Serial.println(drivestate);
		Serial.println(watts);
		Serial.println(rpm);
		Serial.println(ppc);
		Serial.println(err);
		Serial.println(timer);

		_currentPumpStatus[pump].run = run;
		_currentPumpStatus[pump].mode = mode;
		_currentPumpStatus[pump].drivestate = drivestate;
		_currentPumpStatus[pump].watts = watts;
		_currentPumpStatus[pump].rpm = rpm;
		_currentPumpStatus[pump].ppc = ppc;
		_currentPumpStatus[pump].err = err;
		_currentPumpStatus[pump].timer = timer;
	}
	else {
		Serial.println("Updating fields...");

		if (significantWattsChange(pump, watts, counter) || _currentPumpStatus[pump].run != run || _currentPumpStatus[pump].mode != mode) {
			needToEmit = 1;
		}

		if (_currentPumpStatus[pump].run != run) {
			whatsDifferent += 'Run: ' + _currentPumpStatus[pump].run + '-->' + run + ' ';
			_currentPumpStatus[pump].run = run;
			needToEmit = 1;
		}
		if (_currentPumpStatus[pump].mode != mode) {
			whatsDifferent += 'Mode: ' + _currentPumpStatus[pump].mode + '-->' + mode + ' ';
			_currentPumpStatus[pump].mode = mode;
			needToEmit = 1;
		}
		if (_currentPumpStatus[pump].drivestate != drivestate) {
			whatsDifferent += 'Drivestate: ' + _currentPumpStatus[pump].drivestate + '-->' + drivestate + ' ';
			_currentPumpStatus[pump].drivestate = drivestate;
		}
		if (_currentPumpStatus[pump].watts != watts) {
			whatsDifferent += 'Watts: ' + _currentPumpStatus[pump].watts + '-->' + watts + ' ';
			_currentPumpStatus[pump].watts = watts;
		}
		if (_currentPumpStatus[pump].rpm != rpm) {
			whatsDifferent += 'rpm: ' + _currentPumpStatus[pump].rpm + '-->' + rpm + ' ';
			_currentPumpStatus[pump].rpm = rpm;
		}
		if (_currentPumpStatus[pump].ppc != ppc) {
			whatsDifferent += 'ppc: ' + _currentPumpStatus[pump].ppc + '-->' + ppc + ' ';
			_currentPumpStatus[pump].ppc = ppc;
		}
		if (_currentPumpStatus[pump].err != err) {
			whatsDifferent += 'Err: ' + _currentPumpStatus[pump].err + '-->' + err + ' ';
			_currentPumpStatus[pump].err = err;
		}
		if (_currentPumpStatus[pump].timer != timer) {
			whatsDifferent += 'Timer: ' + _currentPumpStatus[pump].timer + '-->' + timer + ' ';
			_currentPumpStatus[pump].timer = timer;
		}
		Serial.println("Different:");
		Serial.println(whatsDifferent);

		//if (s.logPumpMessages)
		//	logger.verbose('\n Msg# %s  %s Status changed %s : ', counter, container.constants.ctrlString[pump + 95], whatsDifferent, data, '\n');

	}
	if (needToEmit == 1) {
		//container.io.emitToClients('pump');
		if (_callback != NULL) {
			_callback(_currentPumpStatus[pump]);
		}
	}
}
bool PumpHolder::significantWattsChange(int pump, int watts, int counter) {
	if ((abs((watts - _currentPumpStatus[pump].watts) / watts)) > .05) {
		//if (s.logPumpMessages) logger.info('Msg# %s   Pump %s watts changed >5%: %s --> %s \n', counter, pump, currentPumpStatus[pump].watts, watts)
		return true;
	}
	return false;
}
void PumpHolder::setCallback(pump_callback callback) {
	_callback = callback;
}

//*******************************************************************************//
// Pentair methods
//*******************************************************************************//
Pentair::Pentair(int rxPin, int txPin) {
	_serial = new SoftwareSerial(rxPin, txPin);
	_serial->begin(9600);
	bufferToProcess = LinkedList<byte>();
	chatter = LinkedList<byte>();

	preambleStd[0] = 255;	preambleStd[1] = 165;	preambleChlorinator[0] = 16;
	preambleChlorinator[1] = 2;
}


//*******************************************************************************//
// Reading the RS485 Bus
//*******************************************************************************//

// check https://github.com/tagyoureit/nodejs-poolController/blob/master/lib/comms/inbound/receive-buffer.js
// -> from serial port -> packetbuffer::push ->
// -> iterateOverArrayOfArrays 
// ->	processChecksum
// ->		checksum
// ->			isResponse
// ->			decode
// ->				processPumpPacket
// ->					run logic from inbound/pump1..7

// Takes in the RS485 bytes and tries to parse them.
void Pentair::ProcessIncommingSerialMessages() {
	ReadIntoBuffer();
	AnalyseCurrentBuffer();
	if (breakLoop) {
		if (debugLog) Serial.println("iOAOA: Exiting because breakLoop");
		return;
	}
	else if (bufferToProcess.size() > 0) {
		//if (s.logMessageDecoding)
		if (debugLog) Serial.println("iOAOA: Recursing back into iOAOA because no bufferToProcess.size() > 0");
		ProcessIncommingSerialMessages();
	}
	else if (bufferToProcess.size() == 0) {
		//processingBuffer.processingBuffer = false;
		//if (s.logMessageDecoding)
		if (debugLog) Serial.println("iOAOA: Exiting out of loop because no further incoming buffers to append.bufferToProcess.size() == 0");
	}
	else {
		//if (s.logMessageDecoding)
		if (debugLog) Serial.println("iOAOA: Recursing back into iOAOA because no other conditions met.");
		ProcessIncommingSerialMessages();
	}
}

void Pentair::ReadIntoBuffer() {
	if (_serial->available() > 0) {
		analogWrite(4, 255);
		int len = _serial->available();
		for (int i = 0; i < len; i++) {
			bufferToProcess.add(_serial->read());
		}
		analogWrite(4, 0);
	}
	else {
		return;
		if (bufferToProcess.size() == 0) {
			bufferToProcess = LinkedList<byte>();
			//   PACKET FORMAT    <------------------NOISE---------------->  <------PREAMBLE------>  Sub   Dest  Src   CFI   Len   Dat1  Dat2  ChkH  ChkL     //Checksum is sum of bytes A5 thru Dat2.
			// byte poolOn[] =    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xA5, 0x07, 0x10, 0x20, 0x86, 0x02, 0x06, 0x01, 0x01, 0x6B };
			// fillup with some dummy data (for testing)
			Serial.println(F("Going to add noise and message(s)"));
			bufferToProcess.add(0xFF);
			bufferToProcess.add(0xFF);
			bufferToProcess.add(0xFF);
			bufferToProcess.add(0xFF);
			bufferToProcess.add(0x00);
			bufferToProcess.add(0xFF);
			bufferToProcess.add(0xA5);// 0
			bufferToProcess.add(0xFF);
			bufferToProcess.add(0xFF);
			bufferToProcess.add(0xFF);

			bufferToProcess.add(0xFF);
			bufferToProcess.add(0x00);
			bufferToProcess.add(0xFF);
			bufferToProcess.add(0xA5);// 0

			bufferToProcess.add(0x07);//sub
			bufferToProcess.add(0x10);//dst 
			bufferToProcess.add(0x60);//src //pump 1 = 0x60
			bufferToProcess.add(0x86);//cfi action
			bufferToProcess.add(0x02);//len
			bufferToProcess.add(0x06);//dat1
			bufferToProcess.add(0x01);//dat2
			bufferToProcess.add(0x01);//chkh
			bufferToProcess.add(0x6B);//chkl

			bufferToProcess.add(0xFF);
			bufferToProcess.add(0xFF);
			bufferToProcess.add(0xFF);
			bufferToProcess.add(0xFF);
			bufferToProcess.add(0xFF);
			bufferToProcess.add(0xFF);

			//				    <------PREAMBLE------>  Sub   Dest  Src   CFI   Len   Dat1  Dat2																				ChkH  ChkL     //Checksum is sum of bytes A5 thru Dat2.
			byte response[] = { 0xFF, 0x00, 0xFF, 0xA5, 0x00, 0x10, 0x60, 0x07, 0x0F, 0x0A, 0x00, 0x00, 0x00, 0xF5, 0x05, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x17, 0x34, 0x02, 0xF3 }; //  (from pump with stats)
			for (int i = 0; i < 26; i++)
			{
				bufferToProcess.add(response[i]);
			}

			Serial.print("Added "); Serial.print(bufferToProcess.size()); Serial.println(" bytes.");
		}
		delay(2000);
	}
}

void Pentair::AnalyseCurrentBuffer() {
	if (debugLog) Serial.print("Break-loop: ");
	if (debugLog) Serial.println(breakLoop);
	while (bufferToProcess.size() > 0 && !breakLoop) {
		if (debugLog) Serial.print(bufferToProcess.get(0), HEX);
		if (debugLog) Serial.print(" ");
		if (debugLog) Serial.println(bufferToProcess.get(1), HEX);

		if (preambleStd[0] == bufferToProcess.get(0) && preambleStd[1] == bufferToProcess.get(1)) {	 // match on pump or controller packet
																									 // 0xFF 0xA5
			if (debugLog) Serial.println("Possible match...");
			int chatterlen = -1;
			if (bufferToProcess.size() >= 7) {
				chatterlen = bufferToProcess.get(6) + 6 + 2; //chatterlen is length of following message not including checksum (need to add 6 for start of chatter, 2 for checksum)
			}
			//   0,   1,      2,      3,    4, 5,        6
			//(255,165,preambleByte,Dest,Src,cmd,chatterlen) and 2 for checksum)
			if (chatterlen >= 50) {
				// we should never get a packet greater than or equal to 50.  So if the chatterlen is greater than that let's shift the array and retry
				//if (s.logMessageDecoding) logger.debug('iOAOA: Will shift first element out of bufferToProcess because it appears there is an invalid length packet (>=50) Lengeth: %s  Packet: %s', bufferToProcess[6], bufferToProcess)
				//bufferToProcess.shift() //remove the first byte so we look for the next [255,165] in the array.
				ShiftBuffer(1);
			}
			else if ((bufferToProcess.size() - chatterlen) <= 0) {
				if (debugLog) Serial.println(F("Msg#  n/a   Incomplete message in bufferToProcess. => breakLoop"));
				breakLoop = true;  //do nothing, but exit until we get a second buffer to concat
			}
			else if (chatterlen == -1) {
				if (debugLog) Serial.println("iOAOA: Setting breakLoop=true because isNan(chatterlen)");
				breakLoop = true;  //do nothing, but exit until we get a second buffer to concat
			}
			else {
				if (debugLog) Serial.println(F("iOAOA: Think we have a packet."));
				msgCounter++;
				ShiftBuffer(1); // remove the 255 byte -> 0xA5 is byte 0

				if (chatterLength != -1) {
					if (debugLog) Serial.println("iOAOA: Clearing chatter...");
					//chatter.clear();	// cleanup first
					chatter = LinkedList<byte>();
					if (debugLog) Serial.println("iOAOA: Cleared chatter...");
				}
				chatterLength = chatterlen;
				for (int i = 0; i < chatterlen; i++) {
					//chatter[i] = bufferToProcess[i];
					chatter.add(bufferToProcess.get(i));
				}
				if (debugLog) Serial.println("iOAOA: Copied chatter");
				ShiftBuffer(chatterlen);	// remove chatter
											//chatter = bufferToProcess.splice(0, chatterlen); //splice modifies the existing buffer.  We remove chatter from the bufferarray.
				if (debugLog) Serial.println("iOAOA: removed chatter from buffer");

				if (((chatter.get(packetFields_DEST) == PUMP1 || chatter.get(packetFields_DEST) == PUMP2))
					|| chatter.get(packetFields_FROM) == PUMP1 || chatter.get(packetFields_FROM) == PUMP2) {
					packetType = PumpPacket;// 'pump'
				}
				else {
					packetType = ControllerPacket; //'controller';
												   //container.intellitouch.setPreambleByte(chatter[1]); //we dynamically adjust this based on what the controller is using.  It is also different for the pumps (should always be 0 for pump messages)
				}
				if (debugLog) Serial.println(F("Incomming packet ready for decoding"));
				ProcessChecksum();
				if (debugLog) Serial.println("Processed packet");
				if (debugLog) Serial.print("Buffer size left: ");
				if (debugLog) Serial.println(bufferToProcess.size());
			}
		}
		else {	//not a preamble for chlorinator or pump/controller packet.  Eject the first byte.
			if (debugLog) Serial.print("-> ");
			if (debugLog) Serial.println(bufferToProcess.size());
			ShiftBuffer(1);
		}
	}
}

void Pentair::ProcessChecksum() {
	int counter = msgCounter;

	if (CheckSum()) {
		Decode(); //chatter, counter, packetType
	}
}

bool Pentair::CheckSum() {
	LinkedList<byte> chatterCopy = LinkedList<byte>();
	for (int i = 0; i < chatterLength; i++) {
		chatterCopy.add(chatter.get(i));
	}
	int len = chatterLength;

	int chatterdatachecksum = 0;
	int databytes = 0;

	//checksum is calculated by 256*2nd to last bit + last bit
	chatterdatachecksum = (chatterCopy.get(len - 2) * 256) + chatterCopy.get(len - 1);
	// add up the data in the payload
	for (int i = 0; i < len - 2; i++) {
		databytes += chatterCopy.get(i);
	}

	bool validChatter = (chatterdatachecksum == databytes);
	if (!validChatter) {
		checksumMismatchCounter++;

		if (checksumMismatchCounter == 1) {
			if (debugLog) Serial.println("Msg#  Always get a first mismatch when opening the port.  Ignoring.");
		}
		else {
			if (debugLog) Serial.print("Msg# Packet collision on bus detected. (Count of collissions: ");
			if (debugLog) Serial.print(checksumMismatchCounter);
			if (debugLog) Serial.println(")");
			//Serial.println('Msg# Mismatch #%s on checksum:   %s!=%s   %s', counter, countChecksumMismatch.counter, chatterdatachecksum, databytes, chatterCopy);
		}
	}
	else {
		if (debugLog) Serial.println("Msg# Match on Checksum:");
	}
	return validChatter;
}

void Pentair::Decode() {
	bool decoded = false;
	//when searchMode (from socket.io) is in 'start' status, any matching packets will be set to the browser at http://machine.ip:3000/debug.html
	/*if (container.apiSearch.searchMode == "start") {
	string resultStr = "Msg#: " + counter + " Data: " + JSON.stringify(data);
	if (container.apiSearch.searchAction == = data[container.constants.packetFields.ACTION] && container.apiSearch.searchSrc == = data[container.constants.packetFields.FROM] && container.apiSearch.searchDest == = data[container.constants.packetFields.DEST]) {
	container.io.sockets.emit('searchResults', resultStr);
	}
	}*/

	//container.intellitouch.checkIfNeedControllerConfiguration()

	//if (s.logMessageDecoding) logger.silly('Msg# %s  TYPE %s,  packet %s', counter, packetType, data);

	//Start Controller Decode

	//I believe this should be any packet with 165,10.  Need to verify. --Nope, 165,16 as well.

	//if (packetType == = 'controller') {
	//	decoded = container.processController.processControllerPacket(data, counter)
	//}

	//Start Pump Decode
	//I believe this should be any packet with 165,0.  Need to verify.
	/*else*/
	if (packetType == PumpPacket) {	// pump

		decoded = ProcessPumpPacket();//chatter, msgCounter
	}
	//Start Chlorinator Decode
	//else if (packetType == = 'chlorinator') {
	//	decoded = container.processChlorinator.processChlorinatorPacket(data, counter)
	//}
	//in case we get here and the first message has not already been set as the instruction command

	/*if (!decoded) {
	if (s.logConsoleNotDecoded) {
	logger.info('Msg# %s is NOT DECODED %s', counter, JSON.stringify(data));
	};
	}
	else {
	decoded = false
	}*/
}

// got an incomming packet from the pump
bool Pentair::ProcessPumpPacket() {
	bool decoded = false;
	Serial.print("Decoding a pump packet -> ");
	Serial.println(chatter.get(packetFields_ACTION));

	switch (chatter.get(packetFields_ACTION)) {
	case 1:	// Set speed setting
		Process_pump_1();	// data, counter
		decoded = true;
		break;
	case 2: //??
	{
		Process_pump_2();	// data, counter
		decoded = true;
		break;
	}
	case 4: //Pump control panel on/off
	{
		Process_pump_4();	// data, counter
		decoded = true;
		break;
	}
	case 5: //Set pump mode
	{
		Process_pump_5();	// data, counter
		decoded = true;
		break;
	}
	case 6: //Turn pump on/off
	{
		Process_pump_6();	// data, counter
		decoded = true;
		break;
	}
	case 7: //cyclical status of pump requesting pump status
	{
		//container.common_7.process(data, counter)
		Process_common_7();
		decoded = true;
		break;
	}
	default:
		Serial.println("UNKNOWN action for pump packet");
		break;
	}
	return decoded;
}

void Pentair::Process_pump_1() {	// uses data and msgCounter
	int pump = chatter.get(packetFields_FROM);

	int setAmount = chatter.get(8) * 256 + chatter.get(9);
	if (chatter.get(packetFields_LENGTH) == 2) {  // Length==2 is a response.
		_pump.pumpAck(chatter, chatterLength, pump, msgCounter);
	}
	else if (chatter.get(pump_packetFields_CMD) == 3) {
		switch (chatter.get(pump_packetFields_MODE))
		{
		case 33: // 0x21
		{
			int program = setAmount / 8;
			_pump.setCurrentProgramFromController(program, pump, chatter, msgCounter);
			break;
		}
		case 39: // 0x27
		{
			int program = 1;
			int rpm = setAmount;
			_pump.saveProgramAs(program, rpm, pump, chatter, msgCounter);
			break;
		}
		case 40: //0x28
		{
			int program = 2;
			int rpm = setAmount;
			_pump.saveProgramAs(program, rpm, pump, chatter, msgCounter);
			break;
		}
		case 41: //0x29
		{
			int program = 3;
			int rpm = setAmount;
			_pump.saveProgramAs(program, rpm, pump, chatter, msgCounter);
			break;
		}
		case 42: //0x2a
		{
			int program = 4;
			int rpm = setAmount;
			_pump.saveProgramAs(program, rpm, pump, chatter, msgCounter);
			break;
		}
		case 43:
		{
			//commented out the following line because we are not sure what the timer actually does
			//leaving it in creates problems for ISY that might rely on this variable
			//pumpStatus.timer = setAmount;
			break;
		}
		default:
			break;
		}
	}
	else if (chatter.get(pump_packetFields_CMD) == 2) {  // data[4]: 1== Response; 2==IntelliTouch; 3==Intellicom2(?)/manual
														 //logger.verbose('Msg# %s   %s --> %s: Set Speed to %s rpm: %s', 
														 //		counter, container.constants.ctrlString[data[container.constants.packetFields.FROM]], container.constants.ctrlString[data[container.constants.packetFields.DEST]], setAmount, JSON.stringify(data));
		Serial.print("Msg#: Set Speed to ");
		Serial.print(setAmount);
		Serial.println(" rpm.");
	}
	else {
		Serial.println("WARN: Pump data?");
	}
}

void Pentair::Process_pump_2() {
	Serial.println("in pump area 2");
}

void Pentair::Process_pump_4() {
	bool remoteControl = false;
	if (chatter.get(pump_packetFields_CMD) == 255) {	// Set pump control panel off (Main panel control only)
		remoteControl = true;
	}
	_pump.setRemoteControl(remoteControl, chatter.get(packetFields_FROM), chatter, msgCounter);
}

void Pentair::Process_pump_5() {
	byte mode = chatter.get(pump_packetFields_MODE);
	_pump.setRunMode(mode, chatter.get(packetFields_FROM), chatter, msgCounter);
}

void Pentair::Process_pump_6() {
	bool power = false;
	if (chatter.get(pump_packetFields_CMD) == 10) {
		power = true;
	}
	else if (chatter.get(pump_packetFields_CMD) == 4) {
		power = false;
	}
	_pump.setPowerFromController(power, chatter.get(packetFields_FROM), chatter, msgCounter);
}

void Pentair::Process_common_7() {
	if (chatter.get(pump_packetFields_DEST) == PUMP1 || chatter.get(pump_packetFields_DEST) == PUMP2) {
		_pump.provideStatus(chatter, msgCounter);
	}
	else // response
	{
		int pump;
		if (chatter.get(pump_packetFields_FROM) == PUMP1 || chatter.get(pump_packetFields_DEST) == PUMP1) {
			pump = 1;
		}
		else {
			pump = 2;
		}
		int hour = chatter.get(pump_packetFields_HOUR);
		int min = chatter.get(pump_packetFields_MIN);
		int run = chatter.get(pump_packetFields_CMD);
		int mode = chatter.get(pump_packetFields_MODE);
		int drivestate = chatter.get(pump_packetFields_DRIVESTATE);
		int watts = (chatter.get(pump_packetFields_WATTH) * 256) + chatter.get(pump_packetFields_WATTL);
		int rpm = (chatter.get(pump_packetFields_RPMH) * 256) + chatter.get(pump_packetFields_RPML);
		int ppc = chatter.get(pump_packetFields_PPC);
		int err = chatter.get(pump_packetFields_ERR);
		int timer = chatter.get(pump_packetFields_TIMER);
		_pump.setPumpStatus(pump, hour, min, run, mode, drivestate, watts, rpm, ppc, err, timer, chatter, msgCounter);
	}
}

void Pentair::ShiftBuffer(int count) {
	// remove 'count' items from array and cleanup
	for (int i = 0; i < count; i++) {
		bufferToProcess.shift();
	}
}


//*******************************************************************************//
// Sending over the RS485 Bus
//*******************************************************************************//

// Command functions
void Pentair::PumpStatusCheck(int index) {
	int pump = PumpIndexToAddress(index);
	SetPumpToRemoteControl(pump);
	RequestPumpStatus(pump);
	SetPumpToLocalControl(pump);
}
bool Pentair::PumpCommandSetPower(int index, bool power) {
	int pump = PumpIndexToAddress(index);
	if (pump > -1) {
		SetPumpToRemoteControl(pump);
		//if (container.settings.logApi) logger.verbose('User request to set pump %s power to %s', index, power == = 1 ? 'on' : 'off');
		SendPumpPowerPacket(pump, power);
		EndPumpCommand(pump);
		return true;
	}
	//logger.warn('FAIL: request to set pump %s (address %s) power to %s', index, pump, power == = 1 ? 'on' : 'off');
	return false;
}
bool Pentair::PumpCommandSaveSpeed(int index, int program, int speed) {
	int pump = PumpIndexToAddress(index);
	if (pump > -1 && program >= 1 && program <= 4) {
		//set program packet
		if (speed <= 450 || speed >= 3450 /*|| isNaN(speed) || speed == null*/) {
			//if (container.settings.logApi) logger.warn('Speed provided (%s) is outside of tolerances.  Program being run with speed that is stored in pump.', speed);
			return false;
		}
		else {
			//if (container.settings.logApi) logger.verbose('User request to save pump %s (address %s) to Program %s as %s RPM', pumpAddressToIndex(pump), pump, program, speed);
			SetPumpToRemoteControl(pump);
			SaveProgramOnPump(pump, program, speed);
			EndPumpCommand(pump);
		}
		return true;
	}
	//logger.warn('FAIL: User request to save pump %s (address %s) to Program %s as %s RPM', pumpAddressToIndex(pump), pump, program, speed);
	return false;
}
bool Pentair::PumpCommandRunProgram(int index, int program) {
	int pump = PumpIndexToAddress(index);
	if (pump > -1) {
		if (program >= 1 && program <= 4) {
			//if (container.settings.logApi) logger.verbose('User request to run pump %s (address %s) Program %s for an unspecified duration', index, pump, program);
			SetPumpToRemoteControl(pump);
			RunProgram(pump, program);
			SendPumpPowerPacket(pump, true);
			EndPumpCommand(pump);
			return true;
		}
	}
	//logger.warn('User request to run pump %s (address %s) Program %s for an unspecified duration', index, pump, program);
	return false;
}
bool Pentair::PumpCommandRunProgramForDuration(int index, int program, int duration) {
	int pump = PumpIndexToAddress(index);

	if (pump > -1 && program >= 1 && program <= 4 && duration > 0) {

		//if (container.settings.logApi) logger.verbose('Request to set pump %s to Program %s (address: %s) for %s minutes', index, pump, program, duration);

		SetPumpToRemoteControl(pump);
		RunProgram(pump, program);
		SendPumpPowerPacket(pump, true); //maybe this isn't needed???  Just to save we should not turn power on.
		SetPumpTimer(pump, duration);

		//run the timer update 50s into the 1st minute
		StartTimer(index);

		EndPumpCommand(pump);
		return true;
	}
	//logger.warn('FAIL: Request to set pump %s (address: %s) to Program %s for %s minutes', index, pump, program, duration);
	return false;
}
bool Pentair::PumpSaveAndRunProgramWithSpeedForDuration(int index, int  program, int speed, int duration) {
	int pump = PumpIndexToAddress(index);
	if (pump > -1) {
		if (PumpCommandSaveSpeed(index, program, speed) &&
			PumpCommandRunProgramForDuration(index, program, duration)) {
			//if (container.settings.logApi) logger.verbose('Request to set pump %s to Program %s (address: %s) for @ %s RPM for %s minutes', index, pump, program, speed, duration);
			return true;
		}
	}
	//logger.warn('FAIL: Request to set pump %s (address: %s) to Program %s for @ %s RPM for %s minutes', index, pump, program, speed, duration);
	return false;
}

void Pentair::SetCallback(pump_callback callback) {
	_pump.setCallback(callback);
	_callback = callback;
}

// Command helpers
//set pump to remote control
void Pentair::SetPumpToRemoteControl(int pump) {
	byte remoteControlPacket[] = { 0xA5, 0x00, pump, appAddress, 0x04, 0x01, 0xFF };
	//if (container.settings.logApi) logger.verbose('Sending Set pump to remote control: %s', remoteControlPacket);
	QueuePacket(remoteControlPacket, 7);
}
//turn pump on/off
void Pentair::SendPumpPowerPacket(int pump, bool power) {
	int index = PumpAddressToIndex(pump);
	byte setPrg_[3];
	//if (container.settings.logApi) logger.info('User request to set pump %s to %s', pump, power);
	if (power == false) {
		setPrg_[0] = 0x6;
		setPrg_[1] = 0x1;
		setPrg_[2] = 0x4;
		ClearTimer(index);
	}
	else if (power == true) // pump set to on
	{
		setPrg_[0] = 0x6;
		setPrg_[1] = 0x1;
		setPrg_[2] = 0xA;
	}
	else {
		return;
	}
	_pump.setPower(index, power);
	byte pumpPowerPacket_[] = { 0xA5, 0x00, pump, appAddress };
	byte pumpPowerPacket[3 + 4];
	JoinArray(pumpPowerPacket_, setPrg_, pumpPowerPacket);
	//Array.prototype.push.apply(pumpPowerPacket, setPrg);

	//if (container.settings.logApi) logger.verbose('Sending Turn pump %s %s: %s', pump, power, pumpPowerPacket);
	QueuePacket(pumpPowerPacket, 7);
}
void Pentair::SaveProgramOnPump(int pump, int program, int speed) {
	int index = PumpAddressToIndex(pump);

	//save program on pump
	//set speed
	byte setPrg_[] = { 1, 4, 3, -1, -1, -1 }; // replace -1
	setPrg_[3] = 38 + program;
	setPrg_[4] = floor(speed / 256); // maybe include math.h
	setPrg_[5] = speed % 256;

	byte setProgramPacket_[] = { 165, 0, pump, appAddress };
	byte setProgramPacket[6 + 4];

	JoinArray(setProgramPacket_, setPrg_, setProgramPacket);
	//Array.prototype.push.apply(setProgramPacket, setPrg);

	//logger.info(setProgramPacket, setPrg)
	//if (container.settings.logApi) logger.verbose('Sending Set Program %s to %s RPM: %s', program, speed, setProgramPacket);
	//_pump.saveProgram(index, program, speed);
	QueuePacket(setProgramPacket, 10);
}
//set pump to local control
void Pentair::SetPumpToLocalControl(int pump) {
	byte localControlPacket[] = { 0xA5, 0x00, pump, appAddress, 0x04, 0x01, 0xFF };
	//if (container.settings.logPumpMessages) logger.verbose('Sending Set pump to local control: %s', localControlPacket);
	QueuePacket(localControlPacket, 7);
}
//generic functions that ends the commands to the pump by setting control to local and requesting the status
void Pentair::EndPumpCommand(int pump) {
	SetPumpToLocalControl(pump);
	RequestPumpStatus(pump);
	// if (container.settings.logApi) logger.info('End of Sending Pump Packet \n \n');

	//container.io.emitToClients('pump');
}

//run program packet
void Pentair::RunProgram(int pump, int program) {
	int index = PumpAddressToIndex(pump);
	//run program
	byte runPrg_[] = { 1, 4, 3, 33, 0, 8 * program };
	byte runProgramPacket_[] = { 165, 0, pump, appAddress };
	byte runProgramPacket[6 + 4];
	JoinArray(runProgramPacket_, runPrg_, runProgramPacket);
	//Array.prototype.push.apply(runProgramPacket, runPrg);

	//if (container.settings.logApi) logger.verbose('Sending Run Program %s: %s', program, runProgramPacket)
	_pump.setCurrentProgram(index, program, -1);
	QueuePacket(runProgramPacket, 10);
}

//request pump status
void Pentair::RequestPumpStatus(int pump) {
	byte statusPacket[] = { 0xA5, 0x00, pump, appAddress, 0x07, 0x00 };
	//if (container.settings.logApi) logger.verbose('Sending Request Pump Status: %s', statusPacket);
	QueuePacket(statusPacket, 6);
}

//NOTE: This pump timer doesn't do what we think it does... I think.
void Pentair::SetPumpTimer(int pump, int duration) {
	int index = PumpAddressToIndex(pump);
	byte setTimerPacket[] = { 165, 0, pump, appAddress, 1, 4, 3, 43, 0, 1 };
	//if (container.settings.logApi) logger.info('Sending Set a 30 second timer (safe mode enabled, timer will reset 2x/minute for a total of %s minutes): %s', duration, setTimerPacket);
	_pump.setDuration(index, duration);
	QueuePacket(setTimerPacket, 10);
}

//clear the internal timer for pump control
void Pentair::ClearTimer(int pump) {
	/*if (pump == = 1) {
	pump1Timer.clearTimeout();
	pump1TimerDelay.clearTimeout();
	}
	else {
	pump2Timer.clearTimeout();
	pump2TimerDelay.clearTimeout();
	}*/
}

//set the internal timer for pump controls
void Pentair::StartTimer(int pump) {
	/*if (pump == = 1) {
	pump1Timer.setTimeout(pump1SafePumpMode, '', '30s')
	}
	else {
	pump2Timer.setTimeout(pump2SafePumpMode, '', '30s')
	}*/
}

// Helper methods

int Pentair::PumpIndexToAddress(int index) {
	if (index == 1) {
		return PUMP1;

	}
	else if (index == 2) {
		return PUMP2;
	}
	return -1;
}

int Pentair::PumpAddressToIndex(int pump) {
	if (pump == PUMP1) {
		return 1;
	}
	else if (pump == PUMP2) {
		return 2;
	}
	return -1;
}

void Pentair::JoinArray(byte array1[], byte array2[], byte dest[]) {
	int len1 = (sizeof(array1) / sizeof(byte));
	int len2 = (sizeof(array2) / sizeof(byte));
	int len = len1 + len2;
	dest = new byte[len];
	int x = 0;
	for (int i = 0; i < len1; i++) {
		dest[x++] = array1[i];
	}
	for (int i = 0; i < len2; i++) {
		dest[x++] = array2[i];
	}
}

// https://github.com/tagyoureit/nodejs-poolController/blob/master/lib/comms/outbound/queue-packet.js
// queuePacket is a sending mechanism that add's checksum and preambles
void Pentair::QueuePacket(byte message[], int messageLength) {
	if (debugLog) Serial.println("queuePacket: Adding checksum and validating packet to be written ");
	//Serial.println(message);

	if (debugLog) Serial.print("queuePacket: message length: ");
	if (debugLog) Serial.println(messageLength);

	//int response = [];

	int checksum = 0;
	for (int j = 0; j < messageLength; j++) {
		checksum += message[j];
	}

	byte* packet;
	int packetSize = 0;
	int requestGet = 0;

	if (message[0] == 16 && message[1] == CHLORINATOR) {
		byte add[3] = { checksum, 16, 3 };
		packetSize = messageLength + 3;
		packet = new byte[packetSize];
		JoinArray(message, add, packet);		// extend the array (to be tested)
		Serial.println("chlorinator packet configured as: "/*, packet*/);
	}
	else {
		// Process the packet to include the preamble and checksum
		//int addInMsg[2] = { checksum >> 8, checksum & 0xFF };

		packetSize = messageLength + 3 + 2;
		packet = new byte[packetSize];

		// Add checksum
		packet[packetSize - 2] = checksum >> 8;
		packet[packetSize - 1] = checksum & 0xFF;

		// Add preamble
		//packet = new int[3]{ 255, 0, 255 };
		packet[0] = 255;
		packet[1] = 0;
		packet[2] = 255;

		// overload message itself
		int offset = 3;
		for (int i = 0; i < packetSize - 5; i++) {
			packet[offset++] = message[i];
		}

		//if we request to "SET" a variable on the HEAT STATUS
		if (packet[7] == 136 && /*s.intellitouch*/ false) {
			requestGet = 1;
		}
	}

	//-------Internally validate checksum
	int len = 0, packetchecksum = 0, databytes = 0;

	if (message[0] == 16 && message[1] == CHLORINATOR) //16,2 packet
	{
		//example packet: 16,2,80,0,98,16,3
		len = packetSize;
		//checksum is calculated by 256*2nd to last bit + last bit
		packetchecksum = packet[len - 3];
		databytes = 0;
		// add up the data in the payload
		for (int i = 0; i < len - 3; i++) {
			databytes += packet[i];
		}
	}
	else //255,0,255,165 packet
	{
		//example packet: 255,0,255,165,10,16,34,2,1,0,0,228
		len = packetSize;
		//checksum is calculated by 256*2nd to last bit + last bit
		packetchecksum = (packet[len - 2] * 256) + packet[len - 1];
		databytes = 0;
		// add up the data in the payload
		for (int i = 3; i < len - 2; i++) {
			databytes += packet[i];
		}
	}
	bool validPacket = (packetchecksum == databytes);
	if (!validPacket) {
		Serial.println("Asking to queue malformed packet:");
		/*response.text = 'Pump packet queued: ' + packet
		response.status = 'error'*/
	}
	else {
		// send packet out
		_serial->write(packet, packetSize);
		delay(100);
		//pump packet
		if (packet[packetFields_DEST + 3] == PUMP1 || packet[packetFields_DEST + 3] == PUMP2) {
			//Serial.println("Just Sent Pump Message '%s' to send: %s", container.constants.strPumpActions[packet[container.constants.packetFields.ACTION + 3]], packet)
			//response.text = 'Pump packet queued: ' + packet
			//response.status = 'ok'
			Serial.print("Just Sent Pump Message, length: ");
			Serial.println(packetSize);
		}
	}
	delete(packet);
}
