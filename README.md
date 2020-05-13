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

## Implementation
Each Service and Characteristic needs a UUID.  For the industry standard ones, you reference the official specifications, but my project here is "user defined".  There's a website that lets you generate UUIDs (https://www.uuidgenerator.net/)

The arduino BLE libraries let you specify Callbacks for each service and characteristic.  I've currently got a "connect and disconnect" callback for the server, and then "Write" callbacks for the characteristcs that set the appropriate cylon variables.

In setup, I'm:
* Initializing my device (which sets it's name for discovery)
* Setting up the Service (along with it's callbacks)
* Setting up the Characteristics and attaching them to the service.

I'm processing the "speed" characteristic as a single byte.

The eye and background color I'm processing as three bytes:  0-255 for R, G, and B.

## Rough Functionality Test
I started test by installing a BLE scanner app on my mac.  There are loads of them for phones as well, but I used BlueSee.

Click the "scan" button, and look for your device in the list.  
Click on that device, and then click connect.  You'll see the UUID for it's service listed on the bottom left.  Click on that, and you'll see the three UUIDs for the characteristics.  

This isn't very user-friendly, as the UUIDs are pretty cryptic...but then you can write bytes to the characteristics and see the changes take effect on the esp32.  The serial monitor will also give some insight into how the device is behaving.
