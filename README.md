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

* void PumpStatusCheck(int index) = Request the pump status
* bool PumpCommandSetPower(int index, bool power) = function to turn the pump on/off 
* bool PumpCommandSaveSpeed(int index, int program, int speed) = function to save the program & speed
* bool PumpCommandRunProgram(int index, int program) = function to run a program
* bool PumpCommandRunProgramForDuration(int index, int program, int duration) = function to run a program for a specified duration
* bool PumpSaveAndRunProgramWithSpeedForDuration(int index, int  program, int speed, int duration) = function to save and run a program with speed for a duration
* void SetCallback(pump_callback callback) = method to call when a pump status has been changed.

## Credits

Credits to :

* http://www.sdyoung.com/home/pool-status/how-i-control-the-pool/
* https://github.com/dminear/easy-touch-raspberry-pi 
* https://github.com/tagyoureit/nodejs-poolController
* https://github.com/tagyoureit/nodejs-Pentair
