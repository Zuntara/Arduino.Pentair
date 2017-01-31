# Arduino.Pentair
Arduino code for Pentair Intelliflo Wisperflo VSD

## How to install
* Copy the contents of the libraries folder to your Arduino folder under and merge with the existing libraries folder.
* Copy the pentair_driver_v2 to the arduino folder
* Open the .ino file from the pentair_driver_v2 folder in your Arduino IDE (you can remove the calls to the analogWrite)

## Notes

I still need to test this on a real system, 
my own pentair pump sends nothing over the RS485 wire, can someone confirm me how to connect it directly to the pump?
- Updated in the meanwhile with issue [Figure out the connection to the intelliflo vsd pump](https://github.com/Zuntara/Arduino.Pentair/issues/1) > the pump start talking when you identified the program as being a valid controller, it also requires a cyclic communication.

Found that the RS-485 pins are pin 6 and 7 of the pump connector. 
Which is the most-bottom pin of the connector and the most-bottom-left one.

This library is simplified to just control the pump(s) - no chlorinator etc. 

Through the callback function you can submit the pump data to where-ever you want.

## Contribute

Feel free to contribute to this library.

You may create pull requests for other commands to the pump, I also need to create an interface to communicate with a phone or app on the desktop over bluetooth or a network.

## Usage

Initialize the Pentair class and give the RX and TX pin numbers of your RS485 module.

call ProcessIncommingSerialMessages()  in the loop() function to retrieve the bytes from the RS485 interface, 
the library will then parse it and call the defined callback function (through 'SetCallback(PumpChanged)' for example)
which will hold the pump information.

Commands that are available:

    // Request the pump status
    void PumpStatusCheck(int index) 
    
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
* https://github.com/tagyoureit/nodejs-Pentair
