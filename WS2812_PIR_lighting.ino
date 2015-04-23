#include <FastLED.h>
#include <Timer.h> 

#define LED_PIN 5
#define LED_COUNT 90
#define DELAY 10

#define SENSOR_PIN 8
#define DEBUG_LED_PIN 13

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
int             off_event           = 0;
int             fade_event          = 0;

CRGB leds[LED_COUNT];                        // Led array

// Deleting if one already started on that integer (id)
void start_after_event(int& event, unsigned int timeout, void (* callback)())
{
  stop_event(event);
  event = t.after(timeout, callback);
}

// Deleting if one already started on that integer (id)
void start_every_event(int& event, void (* callback)())
{
   stop_event(event);
   event = t.every(event, callback);
}

// Stop an event if active
void stop_event(int& event)
{
  if ( event > 0 )
    t.stop(event);
    event = 0;
}

void setup()
{
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, LED_COUNT);
  pinMode(SENSOR_PIN, INPUT);      // sets the SENSOR_PIN as input
  pinMode(DEBUG_LED_PIN, OUTPUT);  // sets the DEBUG_LED_PIN as output

  start_every_event(fade_event, process_fade);
  set_fade_target(CRGB::Beige, 128, 10);

  blink_debug(5, 50);
}

void loop()
{
  if(check_sensor())
  {
    debug_on();
    set_fade_target(CRGB::Beige, 255, 20);
    start_after_event(off_event, ON_TIME, turn_off);      
  }
  else
    debug_off();
  
  t.update();     
  delay(1);
}

void turn_off()
{
  set_fade_target(CRGB::Beige, 0, 20);
}

bool check_sensor()
{
  return digitalRead(SENSOR_PIN) == HIGH;
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

void debug_on()
{
  digitalWrite(DEBUG_LED_PIN, HIGH); 
}

void debug_off()
{
  digitalWrite(DEBUG_LED_PIN, LOW);
}

void blink_debug(byte count, unsigned int pause)
{
  for(int i = 0; i < count; i++)
  {
    debug_on();
    delay(pause);
    debug_off();
    delay(pause);
  }
}

