#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

extern Adafruit_ILI9341 display;
// Function to display the loading animation
void showLoadingScreen(Adafruit_ILI9341 &display);

#endif
