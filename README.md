# Controlling the "stick" with BLE on an esp32

## Definitions
BLE = Bluetooth Low Energy  

Profiles are collections of Services.  See https://www.bluetooth.com/specifications/gatt for official profiles.

Services are collectons of "characteristics".

Characteristics are the lowest level entity.

## Thoughts
I think I'm gonna start by making a "stick control" profile.  
It'll have 8 characteristics, one for each LED.  
Each LED characteristic will be readable (to get it's RGB value) and writeable (to set that RGB value).

Then, I'll tweak an andriod app to set those LEDs.



