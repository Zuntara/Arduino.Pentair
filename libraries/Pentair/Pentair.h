
/*
  AdagioPro.h - Library for Pentair IntelliFlo.
  Created by Filip Slaets, January 23, 2017.
*/
#ifndef Pentair_h
#define Pentair_h

#include "SoftwareSerial.h"
#include "LinkedList.h"

#define appAddress 33
#define CHLORINATOR 2 // constants.js

#define packetFields_DEST  2
#define packetFields_FROM 3
#define packetFields_ACTION 4
#define packetFields_LENGTH 5

#define pump_packetFields_DEST  2
#define pump_packetFields_FROM 3
#define pump_packetFields_ACTION 4
#define pump_packetFields_LENGTH 5
#define pump_packetFields_CMD 6
#define pump_packetFields_MODE 7
#define pump_packetFields_DRIVESTATE 8
#define pump_packetFields_WATTH 9
#define pump_packetFields_WATTL 10
#define pump_packetFields_RPMH 11
#define pump_packetFields_RPML 12
#define pump_packetFields_PPC 13 // ???
#define pump_packetFields_ERR 15
#define pump_packetFields_TIMER 18		// have to explore
#define pump_packetFields_HOUR 19
#define pump_packetFields_MIN 20

#define PUMP1 96  // 0x60
#define PUMP2 97  // 0x61

typedef enum {
	UnknownPacket,
	PumpPacket,
	ControllerPacket
} PacketType;

typedef void(*pump_callback)(class Pump);

// Definition of a Pump with it's properties
class Pump {
public:
	int pump = 0;
	int currentprogram = -1;
	bool remotecontrol = false;
	bool power = false;
	int duration = -1;
	int watts = -1;
	int run = -1;
	int mode = -1;
	int drivestate = -1;
	int rpm = -1;
	int ppc = -1;
	int err = -1;
	int timer = -1;
};

class PumpHolder {
private:
	class Pump _currentPumpStatus[3];
	pump_callback _callback;

public:
	PumpHolder();
	~PumpHolder();

	void pumpAck(LinkedList<byte> data, int dataLen, int pump, int counter);
	void setCurrentProgramFromController(int program, int from, LinkedList<byte> data, int counter);
	int getCurrentProgram(int pump);
	void setCurrentProgram(int pump, int program, int rpm);
	void saveProgramAs(int program, int rpm, int from, LinkedList<byte> data, int counter);
	void setRemoteControl(bool remoteControl, int from, LinkedList<byte> data, int counter);
	void setRunMode(byte mode, int from, LinkedList<byte> data, int counter);
	void setPowerFromController(bool power, int from, LinkedList<byte> data, int counter);
	int getPumpNumber(LinkedList<byte> data);
	void setDuration(int pump, int duration);
	void setPower(int pump, bool power);
	void setTime(int pump, int hour, int min);
	void provideStatus(LinkedList<byte> data, int counter);
	void setPumpStatus(int pump, int hour, int min, int run, int mode, int drivestate, int watts, int rpm, int ppc, int err, int timer, LinkedList<byte> data, int counter);
	bool significantWattsChange(int pump, int watts, int counter);
	void setCallback(pump_callback callback);
};

// Class that will handle Pentair shizzle
class Pentair {
private:
	SoftwareSerial* _serial;
	PumpHolder _pump;
	
	// Reading variables (RS485)
	LinkedList<byte> bufferToProcess;
	bool breakLoop = false;
	byte preambleStd[2];
	byte preambleChlorinator[2];
	PacketType packetType = UnknownPacket;
	LinkedList<byte> chatter;				// potential message on the bus
	int chatterLength = -1;
	int msgCounter = 0;
	int checksumMismatchCounter = 0;

	pump_callback _callback;
public:
	bool debugLog = false;
public:
	// Constructor
	Pentair(int rxPin, int txPin);

	// Read the RS bus for new messages and decode them.
	void ProcessIncommingSerialMessages();

	// Command functions
	// Request the pump status
	void PumpStatusCheck(int index);
	// function to turn the pump on/off 
	bool PumpCommandSetPower(int index, bool power);
	//function to save the program & speed
	bool PumpCommandSaveSpeed(int index, int program, int speed);
	//function to run a program
	bool PumpCommandRunProgram(int index, int program);
	//function to run a program for a specified duration
	bool PumpCommandRunProgramForDuration(int index, int program, int duration);
	//function to save and run a program with speed for a duration
	bool PumpSaveAndRunProgramWithSpeedForDuration(int index, int  program, int speed, int duration);

	void SetCallback(pump_callback callback);

	// ** Methods for retrieving RS485 messages
private:
	// Reads the RS485 connection and appends to the internal buffer.
	void ReadIntoBuffer();
	void AnalyseCurrentBuffer();
	void ProcessChecksum();
	bool CheckSum();
	void Decode();
	bool ProcessPumpPacket();
	void Process_pump_1();
	void Process_pump_2();
	void Process_pump_4();
	void Process_pump_5();
	void Process_pump_6();
	void Process_common_7();
	void ShiftBuffer(int cnt);

	// ** Methods for sending RS485 messages
private:
	// Commanding methods

	//set pump to remote control
	void SetPumpToRemoteControl(int pump);
	//turn pump on/off
	void SendPumpPowerPacket(int pump, bool power);
	//set pump to local control
	void SetPumpToLocalControl(int pump);
	//generic functions that ends the commands to the pump by setting control to local and requesting the status
	void EndPumpCommand(int pump);
	void SaveProgramOnPump(int pump, int program, int speed);
	//run program packet
	void RunProgram(int pump, int program);
	//request pump status
	void RequestPumpStatus(int pump);

	//NOTE: This pump timer doesn't do what we think it does... I think.
	void SetPumpTimer(int pump, int duration);
	//clear the internal timer for pump control
	void ClearTimer(int pump);
	//set the internal timer for pump controls
	void StartTimer(int pump);

	// Helper methods
	int PumpIndexToAddress(int index);
	int PumpAddressToIndex(int pump);
	void JoinArray(byte array1[], byte array2[], byte dest[]);
	// https://github.com/tagyoureit/nodejs-poolController/blob/master/lib/comms/outbound/queue-packet.js
	// queuePacket is a sending mechanism that add's checksum and preambles
	void QueuePacket(byte message[], int messageLength);
};





#endif