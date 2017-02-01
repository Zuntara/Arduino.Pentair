/*
BL121.h - Library for Hanna Instruments Pool Controller BL121.
Created by Filip Slaets, February 1, 2017.
*/

#ifndef BL121_h
#define BL121_h

/*

Analog Output
-------------
The BL121 offers three 4-20 mA outputs for pH, ORP, and temperature. 
Each output may be disabled or connected to an external recording device and is adjustable to be proportional to the measured value. 
For greater flexibility and better resolution, the analog output can be scaled; users can define any two points 
within a parameter range to correspond to the analog output span. This adjustment allows better resolution in the range of interest.


Circulation Pump Monitoring
---------------------------
An inline flow switch or a mechanical relay connected to the recirculation pump power source may be connected to the hold input of the BL121. 
With no flow or when no power is applied to the recirculation pump, the hold circuit disables the dosing pumps. 
This will prevent any dosing of chemical when there is no movement of water in the system.

*/

typedef void(*BL121_AlarmChange)(class BL121Status);

class BL121 {
private:
	// Input
	int _clLevelPin, _phLevelPin, _tempLevelPin;	// A01, A02, A03  on the BL Controller (on analoge pins)
	int _alarmPin;									// Digital pin (high or low)
	// Output
	int _clLowPin, _phLowPin, _holdPin;				// Chlorine, Ph and Hold (stop dosing pumps) pins for output, you will need relays for this
	// Other devices
	//	Contactless liquid sensor
	int _liquidClLowPin, _liquidPhLowPin;
	// State
	BL121Status _status;
	BL121_AlarmChange _alarmCallback
public:
	BL121(BL121_AlarmChange alarmCallback);

	void ConfigureInputPins(int clLevelPin, int  phLevelPin, int  tempLevelPin, int alarmPin);
	void ConfigureOutputPins(int clLowPin, int phLowPin, int holdPin);
	void ConfigureSensorPins(int liquidClLowPin, int liquidPhLowPin);

	void Update();
	BL121Status GetStatus();

private:
	bool IsFaultyState();
};

class BL121Status {
public:
	// Input
	int ClLevel;
	double PhLevel;
	double Temperature;
	bool HasAlarm;

	// Sensors
	bool HasEnoughClLeftInTank;
	bool HasEnoughPhLeftInTank;

	// Notifications
	bool MustNotify;
};

#endif