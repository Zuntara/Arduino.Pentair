/*
BL121.cpp - Library for Hanna Instruments Pool Controller BL121.
Created by Filip Slaets, February 1, 2017.
*/

// For Arduino 1.0 and earlier
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "BL121.h"

//*******************************************************************************//
// BL121 methods
//*******************************************************************************//
BL121::BL121(BL121_AlarmChange alarmCallback) {
	_status = BL121Status();
	_alarmCallback = alarmCallback;
}

//*******************************************************************************//
// Configuration
//*******************************************************************************//
void BL121::ConfigureInputPins(int clLevelPin, int  phLevelPin, int  tempLevelPin, int alarmPin) {
	_clLevelPin = clLevelPin;
	_phLevelPin = phLevelPin;
	_tempLevelPin = tempLevelPin;
	_alarmPin = alarmPin;
}

void BL121::ConfigureOutputPins(int clLowPin, int phLowPin, int holdPin) {
	_clLowPin = clLowPin;
	_phLowPin = phLowPin;
	_holdPin = holdPin;
}

void BL121::ConfigureSensorPins(int liquidClLowPin, int liquidPhLowPin) {
	_liquidClLowPin = liquidClLowPin;
	_liquidPhLowPin = liquidPhLowPin;
}

//*******************************************************************************//
// Execution
//*******************************************************************************//

void BL121::Update() {
	// Read the pool levels
	_status.ClLevel = map(analogeRead(_clLevelPin), 0, 1023, 300.0, 1000.0);	// TODO: check mapping following the range and config in the controller
	_status.PhLevel = map(analogeRead(_phLevelPin), 0, 1023, 2.0, 9.0);
	_status.Temperature = map(analogeRead(_tempLevelPin), 0, 1023, -2.0, 40.0);
	_status.HasAlarm = digitalRead(_alarmPin) == HIGH;

	_status.HasEnoughClLeftInTank = digitalRead(_liquidClLowPin) == HIGH;
	_status.HasEnoughPhLeftInTank = digitalRead(_liquidPhLowPin) == HIGH;

	// TODO: extract into method and take more factors in account.
	if (_status.MustNotify == false && IsFaultyState())
	{
		_status.MustNotify = true;
		_alarmCallback(_status);	// raise alarm callback (Issue)
	}
	else if (_status.MustNotify == true && !IsFaultyState()) {
		_status.MustNotify = false;
		_alarmCallback(_status);	// raise alarm callback (back OK)
	}
}

bool BL121::IsFaultyState() {
	return _status.HasAlarm
		|| _status.ClLevel < 600 || _status.ClLevel > 800
		|| _status.PhLevel < 6.5 || _status.PhLevel > 7.5;
}

BL121Status BL121::GetStatus() {
	return _status;
}