#define USE_ATARI_COLORS
#define SUPPORT_NTSC 1
#define SUPPORT_PAL  1

#include "esp_pm.h"
#include <esp_system.h>

#include <EEPROM.h>

#include "src/gfx/CompositeGraphics.h"
#include "src/gfx/CompositeColorOutput.h"
#include "src/gfx/Image.h"

#include "src/patterns/colorBars.h"

const int VIDEO_FORMAT_PARAM_ADDR = 0;  // EEPROM address to store parameter choice
const int VIDEO_FORMAT_PARAM_SIZE = 4;
int videoFormatValue = CompositeColorOutput::NTSC;

CompositeGraphics graphics(CompositeColorOutput::XRES, CompositeColorOutput::YRES);
CompositeColorOutput composite(CompositeColorOutput::NTSC);

Image<CompositeGraphics> colorBars0(colorBars::xres, colorBars::yres, colorBars::pixels);

const int videoFormatPin = 2;
int buttonState = HIGH;      // Current state of the button
int lastButtonState = HIGH;  // Previous state of the button
unsigned long lastDebounceTime = 0;  // The last time the button state changed
unsigned long debounceDelay = 50;    // Debounce time in milliseconds

void setup() {

  EEPROM.begin(VIDEO_FORMAT_PARAM_SIZE);
  EEPROM.get(VIDEO_FORMAT_PARAM_ADDR, videoFormatValue);
  
  if (videoFormatValue == 1) {
    composite = CompositeColorOutput(CompositeColorOutput::NTSC);
  } else {
    composite = CompositeColorOutput(CompositeColorOutput::PAL);
  }

  esp_pm_lock_handle_t powerManagementLock;
  esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "compositeCorePerformanceLock", &powerManagementLock);
  esp_pm_lock_acquire(powerManagementLock);
  
  composite.init();
  graphics.init();

  pinMode(videoFormatPin, INPUT_PULLUP);
}

void drawImage() {
  graphics.begin(0);

  colorBars0.draw(graphics, 0, 0);
  
  graphics.end();
}

void loop() {
  drawImage();
  composite.sendFrameHalfResolution(&graphics.frame);

  int reading = digitalRead(videoFormatPin);  // Read the state of the button
  
  // Check if the button state has changed
  if (reading != lastButtonState) {
    lastDebounceTime = millis();  // Record the time the button state changed
  }

  // Only change the button state if the debounce time has passed
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // If the button state is LOW, it means the button is pressed
      if (buttonState == LOW) {
        if (videoFormatValue == 0) {
          EEPROM.put(VIDEO_FORMAT_PARAM_ADDR, CompositeColorOutput::NTSC); 
        } else {
          EEPROM.put(VIDEO_FORMAT_PARAM_ADDR, CompositeColorOutput::PAL);  
        }
        EEPROM.commit();
        esp_restart();
      }
    }
  }

  lastButtonState = reading;  // Save the current state as the last state for the next loop
}

