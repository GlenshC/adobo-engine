#include "core/clock.h"
#include "core/platform.h"

#include "util/logger.h"
namespace clk 
{
    TimeState g_time;
    FPSState g_fps;

    void init(void)
    {
        g_time.start = platform::get_time();
        g_time.now = g_time.start;
        g_time.last = g_time.start;
    }
    
    void 
    update(void)
    {
        g_time.last = g_time.now;
        g_time.now = platform::get_time(); 
        g_time.delta = (float) (g_time.now - g_time.last);

        g_fps.timer += g_time.delta;
        g_fps.count++;
        
        if (g_fps.timer >= 1.0)
        {
            g_fps.fps = g_fps.count;
            g_fps.count = 0;
            g_fps.timer = 0.0;
        }
    }
}