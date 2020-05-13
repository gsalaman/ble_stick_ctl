# Controlling the "stick" with BLE (Bluetooth Low Energy) on an esp32
The purpose of this project is to learn how to use BLE to exchange data on an ESP32.  I'm going to attach the "tiny stick" to the esp32 to give it 8 RGB LEDs to tweak.

Phase 1 was just lighting a single RGB LED...that's in the "orig" branch, but is *very* rough.   
Phase 2 is creating a "cylon eye" and an interface to control speed, eye color, and background color.

# Part One:  Porting the Cylon
I've got some decent code that draws the cylon for the "stick" via the Adafruit_NeoPixel Drivers...see my "cylon_v2" repo.  Started by porting that to the esp32, but noticed "flashiness" of the LEDs.  Did a little web research, and it turns out that the Adafruit drivers on the ESP32 tend to do that when other stuff is running...so I migrated to the FastLED drivers.  

I'll leave the port as an excercise to the reader, but there are some cool side-effects:
* we can now use the CRGB 24 bit type
* The "virtual_window" can be of the CRGB type
* I can do some pointer math to make our "real window" the center 8 pixels of the virtual window and initialize the FastLED driver with that!
* I can use FastLED's "fill_gradient" funcitons!!!

# Part Two:  Control
The three parameters I want to control:
* Speed
* Eye Color
* Background Color

## BLE concepts
A peripheral device (like our cylon) is typically a BLE server.  
A server can support multiple Services.   
Each Service has one or more Characteristics.  These are the lowest level "data objects".

For this project, I'm going to make a simple Service that contains 3 Characteristics...one for speed, one for eye color, and one for background color.

Note that the Bluetooth SIG defines "Profiles" as well...these are industry-standard collections of services.  For example, there is a "heart rate monitor" profile that describes which services should be supported in an industry standard heart rate monitor.  Check out https://www.bluetooth.com/specifications/gatt for more info.



