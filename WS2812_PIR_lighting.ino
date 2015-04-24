#include <FastLED.h>
#include <Timer.h> 

#define LED_PIN 5
#define LED_COUNT 90
#define DELAY 10

#define SENSOR_PIN 8
#define DEBUG_LED_PIN 13

#define ON_TIME 25000     // [ms]

enum STATE {
  ON,
  OFF
};

Timer           t;
int             sensor_activated    = 0;
byte            current_brightness  = 0;
byte            target_brightness   = 0;
int             current_blend_progress = 0;
CRGB            target_color        = CRGB::Beige;
unsigned int    fade_speed          = 50;
unsigned int    blend_speed         = 50;
STATE           timer_state         = ON;
int             off_event           = 0;
int             fade_event          = 0;
int             blend_event         = 0;
int             red_event           = 0;
int             green_event         = 0;
int             blue_event          = 0;
int             random_event        = 0;

CRGB leds[LED_COUNT];
byte blend_start_r[LED_COUNT];
byte blend_start_g[LED_COUNT];
byte blend_start_b[LED_COUNT];
byte blend_current_r[LED_COUNT];
byte blend_current_g[LED_COUNT];
byte blend_current_b[LED_COUNT];
byte blend_target_r[LED_COUNT];
byte blend_target_g[LED_COUNT];
byte blend_target_b[LED_COUNT];

// Deleting if one already started on that integer (id)
void start_after_event(int& event, unsigned int timeout, void (* callback)())
{
  stop_event(event);
  event = t.after(timeout, callback);
}

// Deleting if one already started on that integer (id)
void start_every_event(int& event, unsigned int period, void (* callback)())
{
   stop_event(event);
   event = t.every(period, callback);
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
  Serial.begin(115200);
  Serial.flush();
  
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT);
  pinMode(SENSOR_PIN, INPUT);      // sets the SENSOR_PIN as input
  pinMode(DEBUG_LED_PIN, OUTPUT);  // sets the DEBUG_LED_PIN as output

  setLEDs(leds, CRGB::Black);
  showLEDs();
  
  set_fade(255, 10);
  set_blendRandom(50);
  
  blink_debug(5, 50);
}

void set_red()
{
  stop_event(red_event);
  set_blend(CRGB::Red, 30);
}

void set_green()
{
  stop_event(green_event);
  set_blend(CRGB::Green, 30);
}

void set_blue()
{
  stop_event(blue_event);
  set_blend(CRGB::Blue, 30);
}

void set_random()
{
  stop_event(random_event);
  set_blendRandom(30);
}


void loop()
{ 
  if(check_sensor())
  {
    debug_on();
    set_fade(255, 200);

    start_after_event(blue_event, 1, set_blue);
    start_after_event(red_event, 2000, set_red);
    start_after_event(green_event, 5000, set_green);
    start_after_event(random_event, 7500, set_random);
    start_after_event(blue_event, 10000, set_blue);
   
    start_after_event(off_event, ON_TIME, turn_off);      
  }
  else
     debug_off();
  
  t.update();     
}

void turn_off()
{
  stop_event(off_event);
  set_fade(0, 40);
}

bool check_sensor()
{
  return digitalRead(SENSOR_PIN) == HIGH;
}

void set_fade( byte to_brightness, unsigned int fade_speed)
{
  target_brightness = to_brightness;
  start_every_event(fade_event, fade_speed, process_fade);
}

void set_blendRandom(unsigned int blend_speed)
{
  for ( int i = 0; i < LED_COUNT; ++i)
  {
    blend_current_r[i] = leds[i].r;
    blend_current_g[i] = leds[i].g;
    blend_current_b[i] = leds[i].b;
    blend_start_r[i] = leds[i].r;
    blend_start_g[i] = leds[i].g;
    blend_start_b[i] = leds[i].b;
    blend_target_r[i] = random8();
    blend_target_g[i] = random8();
    blend_target_b[i] = random8();
  }
  start_every_event(blend_event, blend_speed, process_blend);
  current_blend_progress = 0;
}

void set_blend(CRGB target_color, unsigned int blend_speed)
{
  for ( int i = 0; i < LED_COUNT; ++i)
  {
    blend_current_r[i] = leds[i].r;
    blend_current_g[i] = leds[i].g;
    blend_current_b[i] = leds[i].b;
    blend_start_r[i] = leds[i].r;
    blend_start_g[i] = leds[i].g;
    blend_start_b[i] = leds[i].b;
    blend_target_r[i] = target_color.r;
    blend_target_g[i] = target_color.g;
    blend_target_b[i] = target_color.b;
  }
  start_every_event(blend_event, blend_speed, process_blend);
  current_blend_progress = 0;
}
  
void process_fade()
{  
  if(current_brightness < target_brightness)
    current_brightness++;
  else if (current_brightness > target_brightness)
    current_brightness--;  
  
  apply_blend(); 
  for ( int i = 0; i < LED_COUNT; ++i )
  {
    leds[i].nscale8( current_brightness );
  }
  showLEDs();
}

void apply_blend()
{
  for ( int i = 0; i < LED_COUNT; ++i )
  {
    leds[i].r = blend_current_r[i];
    leds[i].g = blend_current_g[i];
    leds[i].b = blend_current_b[i];
  }
}

void process_blend()
{
//  Serial.print("current_blend_progress " );
//  Serial.println(current_blend_progress);
//  
//  Serial.print("blend_start:  " );
//  Serial.print(blend_start_r[0]);
//  Serial.print("|" );
//  Serial.print(blend_start_g[0]);
//  Serial.print("|" );
//  Serial.print(blend_start_b[0]);
//  Serial.println("");
//  
//  Serial.print("blend_target:  " );
//  Serial.print(blend_target_r[0]);
//  Serial.print("|" );
//  Serial.print(blend_target_g[0]);
//  Serial.print("|" );
//  Serial.print(blend_target_b[0]);
//  Serial.println("");
//  
//  Serial.print("blend_current: ");
//  Serial.print(blend_current_r[0]);
//  Serial.print("|" );
//  Serial.print(blend_current_g[0]);
//  Serial.print("|" );
//  Serial.print(blend_current_b[0]);
//  Serial.println("");
  
  if (current_blend_progress == 255)
    return;
    
  current_blend_progress++;
  for ( int i = 0; i < LED_COUNT; ++i)
  {
    int r = (int)blend_target_r[i] - (int)blend_start_r[i];
    int g = (int)blend_target_g[i] - (int)blend_start_g[i];
    int b = (int)blend_target_b[i] - (int)blend_start_b[i];
    blend_current_r[i] = blend_start_r[i] + (r*current_blend_progress)/255;
    blend_current_g[i] = blend_start_g[i] + (g*current_blend_progress)/255;
    blend_current_b[i] = blend_start_b[i] + (b*current_blend_progress)/255; 
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

void setLED(CRGB* set_leds, unsigned int index, CRGB color)
{
  set_leds[index] = color;
}

void setLEDs(CRGB* set_leds, CRGB color)
{
  for ( int i = 0; i < LED_COUNT; ++i )
    set_leds[i] = color;
}

void setLEDsRandom(CRGB* set_leds)
{
  for ( int i = 0; i < LED_COUNT; ++i )
  {
    set_leds[i].r = random8();
    set_leds[i].g = random8();
    set_leds[i].b = random8();
  }
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
  for( int i = 0; i < count; ++i )
  {
    debug_on();
    delay(pause);
    debug_off();
    delay(pause);
  }
}

