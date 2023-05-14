/*  
* This script was created by Bob Clagett for I Like To Make Stuff
* For more projects, check out iliketomakestuff.com
* 
* This code is for the project at https://iliketomakestuff.com/
* 
* 
* All of the components are list on the url above.
* 
* We offer NO SUPPORT for this code, and do not promise any updates/improvements.
* Please regard this as "unoptimized, prototype code".
* Feel free to branch and extend this code, but we will likely NOT merge in any changes.
*/
#include <ezButton.h>
#include "Adafruit_Keypad.h"
#include <FastLED.h>

#define LED_PIN 13
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define NUM_LEDS 117

#define BRIGHTNESS 200
#define FRAMES_PER_SECOND 30

bool gReverseDirection = false;

CRGB leds[NUM_LEDS];

const int SEATS = 8;

int seatStart[SEATS] = { 37, 9, 14, 18, 24, 29, 34, 34 };
int seatSize[SEATS] = { 3, 5, 4, 6, 5, 5, 3, 6 };

const int COLORS = 6;
int selectedColor = 0;
CRGB colors[COLORS] = {CRGB::Blue, CRGB::Red, CRGB::Green, CRGB::White, CRGB::Yellow, CRGB::Purple};
CRGB highlightCLR = colors[selectedColor];

CRGBPalette16 gPal;

//-------------------------------------
// keypad setup
//-------------------------------------
const byte ROWS = 4;  // rows
const byte COLS = 4;  // columns
//define the symbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
// byte rowPins[ROWS] = { 5, 4, 3, 2 };    //connect to the row pinouts of the keypad
// byte colPins[COLS] = { 9, 8, 7, 6 };  //connect to the column pinouts of the keypad

byte rowPins[ROWS] = { 2, 3, 4, 5 };    //connect to the row pinouts of the keypad
byte colPins[COLS] = { 6, 7, 8, 9 };  //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int selectedPlayer = 0;
int isRunningAnimation = 0;  
char runningAnimation = '\0'; //(A-D)

void setup() {
  delay(1000);  // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  Serial.begin(9600);

  customKeypad.begin();

  Serial.println("Setup completed!");
}

void loop() {
  random16_add_entropy(random());

  // handle key press
  handleKeyPress();

  // handle selected player
  highlightPlayer();

  // run current animations
  handleAnimations();

  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  delay(10);
}

// KEYPRESSED
void handleKeyPress()
{
  // check keypad
  customKeypad.tick();

  while (customKeypad.available())
  {
    keypadEvent e = customKeypad.read();
    char keypressed = (char)e.bit.KEY;
    if (e.bit.EVENT == KEY_JUST_PRESSED)
    {
      Serial.print("keypressed ");
      Serial.println(keypressed);
      switch (keypressed)
      {
      // clear all lights
      case '0':
        isRunningAnimation = selectedPlayer = 0;
        runningAnimation = '\0';
        highlightCLR = colors[selectedColor];
        FastLED.clear();
        FastLED.show();
        break;
      // players
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
        // case '9':
        highlightCLR = colors[selectedColor];
        selectedPlayer = keypressed - '0';
        Serial.print("selectedPlayer ");
        Serial.println(selectedPlayer);
        // clear old selected player
        FastLED.clear();
        FastLED.show();
        break;
      // animations
      case 'A':
      case 'B':
      case 'C':
      case 'D':
        selectedPlayer = 0;
        if (runningAnimation != keypressed)
        {
          isRunningAnimation = 1;
          runningAnimation = keypressed;
        }
        else
        {
          isRunningAnimation = 0;
          runningAnimation = '\0';
          FastLED.clear();
          FastLED.show();
        }
        break;
      case '*':
        // execute player select
        selectedPlayer += 1;
        if (selectedPlayer > SEATS) {
          selectedPlayer = 1;
        }
        Serial.print("selectedPlayer ");
        Serial.println(selectedPlayer);
        // clear old selected player
        FastLED.clear();
        FastLED.show();
        break;
      case '#':
        selectedColor += 1;
        if (selectedColor >= COLORS) {
          selectedColor = 0;
        }
        highlightCLR = colors[selectedColor];
        break;
      }
    }
  }
}

// ANIMATIONS

void handleAnimations()
{
  if (isRunningAnimation)
  {
    switch (runningAnimation)
    {
    case 'A':
      lightning();
      break;
    case 'B':
      gPal = HeatColors_p;
      Fireplace();
      highlightCLR = CRGB::Blue;
      break;
    case 'C':
      gPal = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::White, CRGB::Green);
      Fireplace();
      highlightCLR = CRGB::Red;
      break;
    case 'D':
      gPal = CRGBPalette16(CRGB::Black, CRGB::DarkGreen, CRGB::Black, CRGB::Yellow);
      Fireplace();
      highlightCLR = CRGB::White;
      break;
    }
  }
}

#define FLASHES 8
#define FREQUENCY 20  // delay between strikes

unsigned int dimmer = 1;

void lightning() {
  for (int flashCounter = 0; flashCounter < random8(3, FLASHES); flashCounter++) {
    if (flashCounter == 0) dimmer = 5;  // the brightness of the leader is scaled down by a factor of 5
    else dimmer = random8(1, 3);        // return strokes are brighter than the leader

    FastLED.showColor(CHSV(255, 0, 255 / dimmer));
    delay(random8(4, 10));  // each flash only lasts 4-10 milliseconds
    FastLED.showColor(CHSV(255, 0, 0));

    if (flashCounter == 0) delay(150);  // longer delay until next flash after the leader
    delay(50 + random8(100));           // shorter delay between strikes
  }
  delay(random8(FREQUENCY) * 100);  // delay between strikes
  highlightPlayer();
}

//from https://codebender.cc/sketch:190691
/* Rate of cooling. Play with to change fire from
   roaring (smaller values) to weak (larger values) 55 */
#define COOLING 55

/* How hot is "hot"? Increase for brighter fire 180 */
#define HOT 120
#define MAXHOT HOT* HEIGHT
#define HEIGHT 1

void Fireplace() {
  static unsigned int spark[NUM_LEDS];  // base heat
  CRGB stack[NUM_LEDS][HEIGHT];         // stacks that are cooler

  // 1. Generate sparks to re-heat
  for (int i = 0; i < NUM_LEDS; i++) {
    if (spark[i] < HOT) {
      int base = HOT * 2;
      spark[i] = random16(base, MAXHOT);
    }
  }

  // 2. Cool all the sparks
  for (int i = 0; i < NUM_LEDS; i++) {
    spark[i] = qsub8(spark[i], random8(0, COOLING));
  }

  // 3. Build the stack
  /*    This works on the idea that pixels are "cooler"
        as they get further from the spark at the bottom */
  for (int i = 0; i < NUM_LEDS; i++) {
    unsigned int heat = constrain(spark[i], HOT / 2, MAXHOT);
    for (int j = HEIGHT - 1; j >= 0; j--) {
      /* Calculate the color on the palette from how hot this
         pixel is */
      byte index = constrain(heat, 0, HOT);
      stack[i][j] = ColorFromPalette(gPal, index);

      /* The next higher pixel will be "cooler", so calculate
         the drop */
      unsigned int drop = random8(0, HOT);
      if (drop > heat) heat = 0;  // avoid wrap-arounds from going "negative"
      else heat -= drop;

      heat = constrain(heat, 0, MAXHOT);
    }
  }

  // 4. map stacks to led array
  for (int i = 0; i < NUM_LEDS; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      leds[(j * NUM_LEDS) + i] = stack[i][j];
    }
  }
}

// PLAYERS

void highlightPlayer() {
  if (selectedPlayer != 0) {
    int startLed = seatStart[selectedPlayer - 1];
    int size = seatSize[selectedPlayer - 1];
    for (int j = startLed; j < (startLed + size); j++)
    {
      leds[j] = highlightCLR;
    }
  }
}
