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

const int REGION_PARAM_ADDR = 0;  // EEPROM address to store parameter choice
const int EEPROM_SIZE = 4;
int regionValue = 0;

CompositeGraphics graphics(CompositeColorOutput::XRES, CompositeColorOutput::YRES);
CompositeColorOutput composite(CompositeColorOutput::NTSC);

Image<CompositeGraphics> colorBars0(colorBars::xres, colorBars::yres, colorBars::pixels);

// Pin where the button is connected
const int buttonPin = 2;

void setup() {

  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(REGION_PARAM_ADDR, regionValue);
  
  //Choose which parameter to use based on the stored value
  if (regionValue == 1) {
    composite = CompositeColorOutput(CompositeColorOutput::PAL);
  } else {
    composite = CompositeColorOutput(CompositeColorOutput::NTSC);
  }

  esp_pm_lock_handle_t powerManagementLock;
  esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "compositeCorePerformanceLock", &powerManagementLock);
  esp_pm_lock_acquire(powerManagementLock);
  
  composite.init();
  graphics.init();

  // Initialize the button pin as input with the internal pull-up resistor enabled
  pinMode(buttonPin, INPUT_PULLUP);
}

void drawImage() {
  graphics.begin(0);

  colorBars0.draw(graphics, 0, 0);
  
  graphics.end();
}

void loop() {
  drawImage();
  composite.sendFrameHalfResolution(&graphics.frame);

  delay(2000);

  if (regionValue == 0) {
    EEPROM.put(REGION_PARAM_ADDR, 1);  // Switch to PAL
  } else {
    EEPROM.put(REGION_PARAM_ADDR, 0);  // Switch to NTSC
  }
    
  // Commit changes to make sure they're saved
  EEPROM.commit();
  esp_restart();  // Reset the ESP32
}
