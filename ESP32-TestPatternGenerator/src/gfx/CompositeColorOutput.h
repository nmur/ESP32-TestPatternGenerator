#include "video_out.h"

class CompositeColorOutput {
    public:
        float pixelAspect = 1.0;
  
        enum Mode {PAL,NTSC};
        static constexpr unsigned int XRES = RawCompositeVideoBlitter::Screen_WIDTH;
        static constexpr unsigned int YRES = RawCompositeVideoBlitter::Screen_HEIGHT;
        
        RawCompositeVideoBlitter::VideoStandard mode;
        
        CompositeColorOutput(Mode mode) : mode(mode == NTSC ? RawCompositeVideoBlitter::NTSC : RawCompositeVideoBlitter::PAL) {
        }
        
        void init() {
            //RawCompositeVideoBlitter::frame_init(); // The CompositeGraphics lib will do this for us
            RawCompositeVideoBlitter::video_init(mode);
        }
        
        void sendFrameHalfResolution(char ***frame) {
            RawCompositeVideoBlitter::_lines = (uint8_t**) *frame;
        }
};
