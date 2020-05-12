
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include <FastLED.h>

#define LED_PIN 12
#define NUMPIXELS 8

#define LED_TYPE WS2812
#define COLOR_ORDER GRB

CRGB leds[NUMPIXELS];



// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHAR_UUID2          "0e4e52c9-3278-49f2-8ed7-9581eb1a8559"

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
        int red;
        int green;
        int blue;
        
        red = value[0];
        green = value[1];
        blue = value[2];
        
        Serial.print("RGB: ");
        Serial.print(red);
        Serial.print(" ");
        Serial.print(green);
        Serial.print(" ");
        Serial.println(blue);  

        leds[0].red = red;
        leds[0].green = green;
        leds[0].blue = blue;
        FastLED.show();
        
      }
      
    }
};

class MyCallbacks2: public BLECharacteristicCallbacks 
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
        int red;
        int green;
        int blue;
        
        red = value[0];
        green = value[1];
        blue = value[2];
        
        Serial.print("RGB: ");
        Serial.print(red);
        Serial.print(" ");
        Serial.print(green);
        Serial.print(" ");
        Serial.println(blue);  

        leds[1].red = red;
        leds[1].green = green;
        leds[1].blue = blue;
        FastLED.show();
        
      }
      
    }
};

void setup() 
{
  int i;
  
  Serial.begin(115200);
  
  FastLED.addLeds<LED_TYPE, 12, COLOR_ORDER>(leds, NUMPIXELS).setCorrection( TypicalLEDStrip );

  fill_solid(leds, NUMPIXELS, CRGB::Black);
  FastLED.show();
  
  
  for (i=0;i<NUMPIXELS;i++)
  {
    leds[i]=CRGB::Blue;
    FastLED.show();
    delay(200);
  }
  delay(1000);

  fill_solid(leds, NUMPIXELS, CRGB::Black);
  FastLED.show();

  Serial.println("two characteristics");

  BLEDevice::init("ESP32 Stick");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
                                       
  BLECharacteristic *pChar2 = pService->createCharacteristic(
                                         CHAR_UUID2,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
                                       
  pCharacteristic->setCallbacks(new MyCallbacks());
  pChar2->setCallbacks(new MyCallbacks2());

  //pCharacteristic->setValue("hello world");
  
  uint8_t init_value[3]={0x00, 0x00, 0x00};
  pCharacteristic->setValue(init_value, 3);
  pChar2->setValue(init_value, 3);
  
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop() 
{  
 
}
