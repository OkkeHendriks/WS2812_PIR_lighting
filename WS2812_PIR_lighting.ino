#include <FastLED.h>
#include <Timer.h> 

#define LED_PIN 5
#define LED_COUNT 90
#define DELAY 10

#define SENSOR_PIN 8
#define DEBUG_PIN 2

#define ON_TIME 2500     // [ms]

enum STATE {
  ON,
  OFF
};

Timer           t;
int             sensor_activated    = 0;
unsigned long   timer               = 0;
unsigned long   fade_timer          = 0;
unsigned int    current_brightness  = 0;
unsigned int    target_brightness   = 0;
CRGB            target_color        = CRGB::Beige;
unsigned int    fade_speed          = 50;
STATE           timer_state         = ON;

CRGB leds[LED_COUNT];                        // Led array

void setup()
{
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, LED_COUNT);
  pinMode(SENSOR_PIN, INPUT);      // sets the digital pin 7 as input
  pinMode(DEBUG_PIN, OUTPUT);      // sets the  pin 7 as input

  clearLEDs();   // This function, defined below, turns all LEDs off...
  showLEDs();
  delay(500);
  allLedsWhite();
  showLEDs();
  delay(500);
  clearLEDs();   // This function, defined below, turns all LEDs off...
  showLEDs();
  delay(500);
  t.every(1, process_fade);
  set_fade_target(CRGB::Beige, 128, 10);

  blink_debug(6);
}

void loop()
{
  if(check_sensor())
  {
    set_fade_target(CRGB::Beige, 255, 20);
    
    // Reset and turn on timer
    timer = 0;
    timer_state = ON;
  }
  
  if(timer_state == ON)
     digitalWrite(DEBUG_PIN, HIGH); 
  else
     digitalWrite(DEBUG_PIN, LOW); 
  
  // If timer > ON_TIME
  if(timer > ON_TIME)
  {
    set_fade_target(CRGB::Beige, 0, 20);
    
    // Reset and turn off timer
    timer = 0;
    timer_state = OFF; 
  }   
  
  if(timer_state == ON)
    timer++;

  t.update();     

  delay(1)  ;

}

bool check_sensor()
{
  return digitalRead(SENSOR_PIN) == HIGH;
}

void blink_debug(byte count)
{
  for(int i = 0; i < 5; i++)
  {
    digitalWrite(DEBUG_PIN, HIGH);
    delay(250);
    digitalWrite(DEBUG_PIN, LOW);
    delay(250);
  }
}

void set_fade_target(CRGB color, int to_brightness, byte wait)
{
  target_color      = color;
  target_brightness = to_brightness;
  fade_speed        = wait;
}
  
void process_fade()
{  
  if (current_brightness == target_brightness)
    return;
    
  if(fade_timer > 0)
  {
    // Timer did not yet expire
    fade_timer--;
    return;      
  }
  else
  {
    // Reset fade timer
    fade_timer = fade_speed;  
    
    if(current_brightness < target_brightness)
      current_brightness++;
    else
      current_brightness--;     
    
    setLEDs(target_color);
    for (int i = 0; i < LED_COUNT; i++)
      leds[i].nscale8( current_brightness );
    showLEDs();
  }
}

void clearLEDs()
{
  FastLED.clear();
}

void allLedsWhite()
{
  // Turn LEDs on 
  for ( int i = 0 ; i < LED_COUNT ; ++i )
    leds[i] = CRGB::White;
}

void setLEDs(CRGB color)
{
  for (int i = 0; i < LED_COUNT; i++)
    leds[i] = color;
}

void showLEDs()
{
    FastLED.show();       // Display LEDs 
}
