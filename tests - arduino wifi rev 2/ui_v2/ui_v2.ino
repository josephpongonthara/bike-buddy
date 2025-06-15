#include <Adafruit_GFX.h> 
#include <SPI.h>   
#include <Adafruit_ILI9341.h> 
#include "TouchScreen.h"     

// not too sure what this part of the code is for, but it's included in the tutorial- maybe remove?
#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// these are same as before for triangle code, but there's a new pin
#define TFT_CS    10
#define TFT_DC    9
#define TFT_RST   8 // not really necessary- this is just a reset pin


// these are touchscreen pins: look at datasheet to assign these!
#define YP A2 //must be an analogue pin - tutorial had A3 for example
#define XM 6 // must be analogue pin
#define YM 7 // digital pin
#define XP A1 // digital pin


Adafruit_ILI9341 tft_screen = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
TouchScreen touch_screen = TouchScreen(XP, YP, XM, YM, 300);

// got from tutorial: this might cause an error since I saw online that not all screens register hex code
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// to handle touchscreen input:
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 3800
#define TS_MAXY 4000

// adjust these as needed to fit our screen:
#define button_x_pos 50
#define button_y_pos 150
// the above first 2 lines contain the x and y location of the button on the screen-think of it as a Cartesian plane point
#define button_width 150
#define button_height 50
// these define the button width and height -  adjust if needed

// screens: main for loadup, ride to confirm user is using bicycle, danger will be where we add back the flashing triangle here
enum screen_state {
  MAIN,
  RIDE_MODE,
  DANGER
};
screen_state currentScreen = MAIN;

void setup() {
  // Initialize TFT screen
  tft_screen.begin();
  tft_screen.setRotation(2);  
  tft_screen.fillScreen(BLACK);  
  // Optional, I thought black background with white text looked nicer 
  
  // Draw the main screen
  drawMainScreen();
}

void loop() {
  // Read touch screen input
  TSPoint p = touch_screen.getPoint();
  
  // If there is a valid touch (z-axis threshold for touch pressure)
  if (p.z > 100) {
    // Map touch coordinates to screen dimensions
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, 320);
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, 240);

    Serial.print("Mapped X: "); Serial.print(p.x);
    Serial.print(", Mapped Y: "); Serial.println(p.y);
    
    if (currentScreen == MAIN) {
      // If touch is inside button on the main screen
      if (p.x > button_x_pos && p.x < button_x_pos + button_width && p.y > button_y_pos && p.y < button_y_pos + button_height) {
        currentScreen = RIDE_MODE;
        drawRideMode();  // Switch to second screen
      }
    } else if (currentScreen == RIDE_MODE) {
      // If touch is inside the back button on the second screen
      if (p.x > button_x_pos && p.x < button_x_pos + button_width && p.y > button_y_pos && p.y < button_y_pos + button_height) {
        currentScreen = MAIN;
        drawMainScreen();  // Switch to main screen
      }
      // add another else-if statement here to handle the flashing screen part
    }
  }
}

void drawMainScreen() {
  tft_screen.fillScreen(BLACK);  
  tft_screen.setTextColor(WHITE);
  tft_screen.setTextSize(2); 
  // adjust this to first screen
  tft_screen.setCursor(50, 50);
  tft_screen.print("IntelliBike");
  
  // button drawing:
  tft_screen.fillRect(button_x_pos, button_y_pos, button_width, button_height, RED);
  tft_screen.setTextColor(WHITE);
  tft_screen.setCursor(button_x_pos + 50, button_y_pos + 10); 
  // orient text to center of button
  tft_screen.print("START!");
}

void drawRideMode() {
  float sample_distance = 20;
  // we can pass the distance from lidar reading here, and see if it can show it onscreen
  tft_screen.fillScreen(BLACK);  
  tft_screen.setTextColor(WHITE);
  tft_screen.setTextSize(2);
  tft_screen.setCursor(50, 50);
  tft_screen.print("Distance tracker: "); tft_screen.println(sample_distance);

  // Draw a button on the second screen (Back button)
  tft_screen.fillRect(button_x_pos, button_y_pos, button_width, button_height, RED);
  tft_screen.setTextColor(WHITE);
  tft_screen.setCursor(button_x_pos + 60, button_y_pos + 10); 
  tft_screen.print("Return Home");
}

// insert the original flashing triangle logic here : void_drawDanger(){}