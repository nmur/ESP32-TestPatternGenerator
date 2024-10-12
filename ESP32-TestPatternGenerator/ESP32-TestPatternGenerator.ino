#define USE_ATARI_COLORS
#define SUPPORT_NTSC 1
#define SUPPORT_PAL  1

#include "esp_pm.h"

#include "src/gfx/CompositeGraphics.h"
#include "src/gfx/CompositeColorOutput.h"
#include "src/gfx/Image.h"

#include "src/patterns/colorBars.h"

CompositeGraphics graphics(CompositeColorOutput::XRES, CompositeColorOutput::YRES);
CompositeColorOutput composite(CompositeColorOutput::NTSC);

Image<CompositeGraphics> colorBars0(colorBars::xres, colorBars::yres, colorBars::pixels);

void setup() {
  esp_pm_lock_handle_t powerManagementLock;
  esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "compositeCorePerformanceLock", &powerManagementLock);
  esp_pm_lock_acquire(powerManagementLock);
  
  composite.init();
  graphics.init();
}

void drawImage()
{
  graphics.begin(0);

  colorBars0.draw(graphics, 0, 0);
  
  graphics.end();
}

void loop() {
  drawImage();
  composite.sendFrameHalfResolution(&graphics.frame);
}
