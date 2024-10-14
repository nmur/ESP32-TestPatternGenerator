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

const int VIDEO_FORMAT_PARAM_ADDR = 0; 
const int VIDEO_FORMAT_PARAM_SIZE = 4;
int videoFormatValue = CompositeColorOutput::NTSC;

CompositeGraphics graphics(CompositeColorOutput::XRES, CompositeColorOutput::YRES);
CompositeColorOutput composite(CompositeColorOutput::NTSC);

Image<CompositeGraphics> colorBars0(colorBars::xres, colorBars::yres, colorBars::pixels);

const int videoFormatPin = 2;

void setup() {
  setVideoFormatFromEeprom();

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

  checkVideoFormatButtonPress();
}

void setVideoFormatFromEeprom() {
  EEPROM.begin(VIDEO_FORMAT_PARAM_SIZE);
  EEPROM.get(VIDEO_FORMAT_PARAM_ADDR, videoFormatValue);
  
  if (videoFormatValue == CompositeColorOutput::NTSC) {
    composite = CompositeColorOutput(CompositeColorOutput::NTSC);
  } else {
    composite = CompositeColorOutput(CompositeColorOutput::PAL);
  }
}

void checkVideoFormatButtonPress() {
  if (digitalRead(videoFormatPin) == LOW) {
    if (videoFormatValue == CompositeColorOutput::PAL) {
      EEPROM.put(VIDEO_FORMAT_PARAM_ADDR, CompositeColorOutput::NTSC); 
    } else {
      EEPROM.put(VIDEO_FORMAT_PARAM_ADDR, CompositeColorOutput::PAL);  
    }
    EEPROM.commit();
    delay(200);
    esp_restart();
  }
}

