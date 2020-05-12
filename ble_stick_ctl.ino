/* controlling the (tiny) stick with an ESP32 over BLE.
 *  
 * This rev imports the cylon-eye from cylon v2.
 * Control is via these characteristics:  
 *   - Speed
 *   - Eye Color (passed as RGB)
 *   - Background Color (passed as RGB).
 */
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include <FastLED.h>

#define LED_PIN 12
#define NUMPIXELS 8

#define LED_TYPE WS2812
#define COLOR_ORDER GRB


#define MAX_EYE_SIZE (NUMPIXELS - 1)

/* Window concepts:
 *  The virtual window is a large array of pixels, but we're only going to 
 *  show the middle 8 pixels...and we'll refer to those as the "real window" 
 *  pixels. 
 */
#define VIRTUAL_WINDOW_SIZE (NUMPIXELS+2*MAX_EYE_SIZE)
CRGB virtual_window[VIRTUAL_WINDOW_SIZE];

#define REAL_WINDOW_START_INDEX MAX_EYE_SIZE 
#define REAL_WINDOW_END_INDEX (REAL_WINDOW_START_INDEX + NUMPIXELS - 1)
CRGB *real_window = &(virtual_window[REAL_WINDOW_START_INDEX]);
CRGB *leds = real_window;

/* The cyclon eye parameters */
int eye_head_pos=REAL_WINDOW_START_INDEX;  // this is the virutal window index
int eye_size=7;

typedef enum
{
  DIR_RIGHT,
  DIR_LEFT
} dir_type;

dir_type current_eye_dir=DIR_RIGHT;

CRGB eye_color=CRGB::Red;
CRGB background_color=CRGB::Blue;

#define DISPLAY_MIN_DELAY 10
#define DISPLAY_MAX_DELAY 1000

uint32_t display_delay_ms=100;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID          "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
// Speed is a one byte characteristic. 0 is the slowest we can go, 255 the fastest
#define SPEED_CHAR_UUID       "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Eye and Background color are three byte RGB values.
#define EYE_COLOR_CHAR_UUID  "0e4e52c9-3278-49f2-8ed7-9581eb1a8559"
#define BACKGROUND_COLOR_CHAR_UUID  "a804d721-ed8f-4a34-82d2-86200391e060"

class SpeedCB: public BLECharacteristicCallbacks 
{
    void onWrite(BLECharacteristic *pCharacteristic) 
    {
      std::string value = pCharacteristic->getValue();

      if (value.length() != 1) 
      {
        Serial.println("Bad Speed Input");
      }
      else
      {     
        display_delay_ms = 10 + 4*value[0];
      }
      
    }
};

class EyeColorCB: public BLECharacteristicCallbacks 
{
    void onWrite(BLECharacteristic *pCharacteristic) 
    {
      std::string value = pCharacteristic->getValue();

      if (value.length() != 3) 
      {
        Serial.println("Bad RGB Input for head");
      }
      else
      {     
        int red;
        int green;
        int blue;
        
        red = value[0];
        green = value[1];
        blue = value[2];
        
        Serial.print("Eye color RGB: ");
        Serial.print(red);
        Serial.print(" ");
        Serial.print(green);
        Serial.print(" ");
        Serial.println(blue);  

        eye_color.red = red;
        eye_color.green = green;
        eye_color.blue = blue;
        
      }
      
    }
};

class BackgroundColorCB: public BLECharacteristicCallbacks 
{
    void onWrite(BLECharacteristic *pCharacteristic) 
    {
      std::string value = pCharacteristic->getValue();

      if (value.length() != 3) 
      {
        Serial.println("Bad RGB Input for background color");
      }
      else
      {     
        int red;
        int green;
        int blue;
        
        red = value[0];
        green = value[1];
        blue = value[2];
        
        Serial.print("Background color RGB: ");
        Serial.print(red);
        Serial.print(" ");
        Serial.print(green);
        Serial.print(" ");
        Serial.println(blue);  

        background_color.red = red;
        background_color.green = green;
        background_color.blue = blue;        
      }     
    }
};
/* Fills the entire virtual window with a given color */
void virtual_fill(CRGB color)
{
    fill_solid(virtual_window, VIRTUAL_WINDOW_SIZE, color);
}

/*=================================================
 * display_pixels
 * 
 * This function displays the "real window" pixels on the 
 * tiny stick.
 */
void display_pixels( void )
{
  FastLED.show();
}

/* this function shows the eye, based on the head position, length, 
 *  and direction
 */
void show_eye( void )
{ 
   int tail_pos;  // virtual window index of the tail

   /* start with the virtual window filled with the background color */  
   virtual_fill(background_color);

   /* Draw in the eye, depending on direction */
   if (current_eye_dir == DIR_RIGHT)
   {
     /* if we're going right, the tail is left of the eye...meaning we 
      *  fillGradient from the tail to the head
      */
     tail_pos = eye_head_pos - eye_size;
     fill_gradient_RGB(&(virtual_window[tail_pos]),
                       eye_size,
                       background_color,
                       eye_color);

     /* if the head of the eye is outside our real window, we want to make the
      *  edge our eye color
      */
     if (eye_head_pos > REAL_WINDOW_END_INDEX)
     {
       virtual_window[REAL_WINDOW_END_INDEX] = eye_color;
     }
   }   // end of "moving right" adjustments
   else
   {
      /* if we're going left, the tail is right of the eye, 
       *  meaning we fill gradient from the head to tail.
       */

      // Note:  we don't really need this in this clause... 
      tail_pos = eye_head_pos + eye_size;

      fill_gradient_RGB(&(virtual_window[eye_head_pos]), 
                        eye_size,
                        eye_color,
                        background_color);
                        
     /* if the head of the eye is outside our real window, we want to make the
      *  edge our eye color
      */
     if (eye_head_pos < REAL_WINDOW_START_INDEX)
     {
       virtual_window[REAL_WINDOW_START_INDEX] = eye_color;
     }    
   }  // end of "moving left" adjustments

   /* Now that we've updated our virtual window with the eye, do the 
    *  actual display
    */
   display_pixels(); 
}

void update_display(void)
{
  static uint32_t last_update_time_ms = 0;
  uint32_t        curr_time_ms;

  curr_time_ms = millis();

  /* if it hasn't been long enough for an update, just return.  */
  if (last_update_time_ms + display_delay_ms > curr_time_ms)
  {
    return;
  }

  last_update_time_ms = curr_time_ms;

  #ifdef PLATFORM_UNO
  Serial.print("eye pos: ");
  Serial.print(eye_head_pos);
  Serial.print(", dir= ");
  Serial.println(current_eye_dir);
  #endif
  
  /* do we need to reverse direction? */
  if ((current_eye_dir == DIR_RIGHT) && 
      (eye_head_pos >= REAL_WINDOW_END_INDEX + eye_size))
  {
    current_eye_dir = DIR_LEFT;

    /* because our head pos is far right in virtual window, we need to reset
     *  it here the the right edge.
     */
    eye_head_pos = REAL_WINDOW_END_INDEX;
  }
  else if ((current_eye_dir == DIR_LEFT) &&
           (eye_head_pos <= REAL_WINDOW_START_INDEX - eye_size))
  {
    current_eye_dir = DIR_RIGHT;

    /* because our head pos is far left in virtual window, we need to reset
     *  it here to the left edge.
     */
    eye_head_pos = REAL_WINDOW_START_INDEX;
  }

  /* move the eye one slot in the desired direction */
  if (current_eye_dir == DIR_RIGHT)
  {
    eye_head_pos++;
  }
  else
  {
    eye_head_pos--;
  }
  
  /* ...and display the eye on our pixels. */
  show_eye();
}


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

  BLEDevice::init("ESP32 Stick Cylon");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pSpeedChar = pService->createCharacteristic(
                                         SPEED_CHAR_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
                                       
  BLECharacteristic *pEyeColorChar = pService->createCharacteristic(
                                         EYE_COLOR_CHAR_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
                                       
  BLECharacteristic *pBackgroundColorChar = pService->createCharacteristic(
                                         BACKGROUND_COLOR_CHAR_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
                                       
  pSpeedChar->setCallbacks(new SpeedCB());
  /* ADD EYE AND BACKGROUND */


  /* FIX ME!!!*/
  uint8_t init_value[3]={0x00, 0x00, 0x00};
  //pCharacteristic->setValue(init_value, 3);
  //pChar2->setValue(init_value, 3);
  
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop() 
{  
  update_display();
}
