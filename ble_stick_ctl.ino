
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include <Adafruit_NeoPixel.h>

#define LED_PIN 12
#define NUMPIXELS 8

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB+NEO_KHZ800);
/* A few helpful color #defines... */
#define COLOR_RED     0xFF0000
#define COLOR_GREEN   0x00FF00
#define COLOR_BLUE    0x0000FF
#define COLOR_MAGENTA 0xFF00FF
#define COLOR_YELLOW  0xFFFF00
#define COLOR_CYAN    0x00FFFF
#define COLOR_BLACK   0
#define COLOR_WHITE   0xFFFFFF

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

int red=0;
int green=0;
int blue=0;

void fillAll( uint32_t color )
{
  int i;

  for (i=0; i<NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}

class MyCallbacks: public BLECharacteristicCallbacks 
{
    void onWrite(BLECharacteristic *pCharacteristic) 
    {
      std::string value = pCharacteristic->getValue();

      if (value.length() != 3) 
      {
        Serial.println("Bad RGB Input");
      }
      else
      {     
        red = value[0];
        green = value[1];
        blue = value[2];
        Serial.print("RGB: ");
        Serial.print(red);
        Serial.print(" ");
        Serial.print(green);
        Serial.print(" ");
        Serial.println(blue);  

      }

      /*
      if (value.length() > 0) {
        Serial.println("*******");
        Serial.print("Length: ");
        Serial.print(value.length());
        Serial.print("  New value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);

        Serial.println();
        Serial.println("*********");
      }
      */
      
    }
};

void setup() 
{
  int i;
  
  Serial.begin(115200);

  pixels.begin();

  for (i=0;i<NUMPIXELS;i++)
  {
    pixels.setPixelColor(i,COLOR_BLUE);
    pixels.show();
    delay(200);
  }
  delay(1000);

  fillAll(COLOR_BLACK);
  

  Serial.println("1- Download and install an BLE scanner app in your phone");
  Serial.println("2- Scan for BLE devices in the app");
  Serial.println("3- Connect to MyESP32");
  Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
  Serial.println("5- See the magic =)");

  BLEDevice::init("ESP32 Stick");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Hello World");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop() 
{
  if (red || green || blue)
  {
        Serial.print(red);
        Serial.print(" ");
        Serial.print(green);
        Serial.print(" ");
        Serial.println(blue); 
  }
  
  pixels.setPixelColor(0, red, green, blue);
  pixels.show();
}
