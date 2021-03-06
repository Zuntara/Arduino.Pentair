# Arduino.Pentair
Arduino code for Pentair Intelliflo Wisperflo VS(D)

## How to install
* Copy the contents of the libraries folder to your Arduino folder under and merge with the existing libraries folder.
* Copy the pentair_driver_v2 to the arduino folder
* Open the .ino file from the pentair_driver_v2 folder in your Arduino IDE (you can remove the calls to the analogWrite)

## Notes

I still need to test this on a real system, 
my own pentair pump sends nothing over the RS485 wire, can someone confirm me how to connect it directly to the pump?
- Updated in the meanwhile with issue [Figure out the connection to the intelliflo vsd pump](https://github.com/Zuntara/Arduino.Pentair/issues/1) > the pump starts talking when you identified the program as being a valid controller, it also requires a cyclic communication.

Found that the RS-485 pins are pin 6 and 7 of the pump connector. 
Which is the most-bottom pin of the connector and the most-bottom-left one.

This library is simplified to just control the pump(s) - no chlorinator etc. 

Through the callback function you can submit the pump data to where-ever you want.

    // set a callback method for when a pump status changes
	pentair.SetCallback(OnPumpChanged);
    
    // Gets called when there is a change in attributes on the given pump
    void OnPumpChanged(Pump myPump) {
	   	
	    Serial.print("Pump "); Serial.print(myPump.pump); Serial.println(" has been changed.");
	    Serial.print("RPM: "); Serial.print(myPump.rpm); Serial.println(" is reported.");
	    
        // Call a HTTP client or send it over TCP/UDP to somewhere if you like....
        
    }

## Contribute

Feel free to contribute to this library.

You may create pull requests for other commands to the pump (if I've missed some), 
I also need to create an interface to communicate with a phone or app on the desktop over bluetooth or a network.

## Usage

Initialize the Pentair class and give the RX and TX pin numbers of your RS485 module (I have the RS-485 Shield from linksprite).

	// Create our pentair instance with SoftwareSerial
	Pentair pentair(6, 7);		// RX, TX pins for RS-485 (shield)
	
	// Create our pentair instance with a HardwareSerial
	Serial1 _serial1;
	Pentair pentair(&_serial1);		// pass the address of the hardware serial

Call *ProcessIncommingSerialMessages()*  in the *loop()* function to retrieve the bytes from the RS485 interface, 
the library will then parse it and call the defined callback function (through '*SetCallback(PumpChanged)*' for example)
which will hold all pump information.

	void loop() {
		// Inspect the bus for incomming messages and process them
		pentair.ProcessIncommingSerialMessages();

		// Wait a little bit (this can be removed i guess)
		delay(20);
	}

Commands that are available:

    // Request the pump status
    void PumpStatusCheck(int index) 
    
    ##
    
    // Turn the pump on/off
    bool PumpCommandSetPower(int index, bool power) 
    
    // Save the program & speed
    bool PumpCommandSaveSpeed(int index, int program, int speed)
    
    // Rrun a fixed preset program
    bool PumpCommandRunProgram(int index, int program)
    
    // Run a program for a specified duration
    bool PumpCommandRunProgramForDuration(int index, int program, int duration)
    
    // Save and run a program with speed for a duration
    bool PumpSaveAndRunProgramWithSpeedForDuration(int index, int  program, int speed, int duration) 
    
    // callback when a pump status has been changed.
    void SetCallback(pump_callback callback) 

## Credits

Credits to :

* http://www.sdyoung.com/home/pool-status/how-i-control-the-pool/
* https://github.com/dminear/easy-touch-raspberry-pi 
* https://github.com/tagyoureit/nodejs-poolController

for giving my enough information to figure out what the pump needs.

## Roadmap

- [ ] Test with a real pump (ongoing)
- [ ] Add remote control via app
- [ ] Integrate Hanna Instruments BL121 controller interaction (for Cl, Ph and Temp readings)
- [ ] Add metrics on the whole system for guarding purposes (Ph levels, Cl levels, temp range, flow control, ...)
- [ ] Make sure everything is pluggable, not everyone has the same system
