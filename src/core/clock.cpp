#include "core/clock.h"
#include "core/platform.h"

#include "util/debug.h"
namespace clk 
{
    TimeState g_time;
    FPSState g_fps;

    void init(void)
    {
        g_time.start = plat::get_time();
        g_time.now = g_time.start;
        g_time.last = g_time.start;
    }
    
    void 
    update(void)
    {
        g_time.last = g_time.now;
        g_time.now = plat::get_time(); 
        g_time.delta = (float) (g_time.now - g_time.last);

        g_fps.timer += g_time.delta;
        g_fps.count++;
        
        if (g_fps.timer >= 1.0)
        {
            g_fps.fps = g_fps.count;
            g_fps.count = 0;
            g_fps.timer = 0.0;
        }

        for(i32 i = 0; i < MAX_FRAME_TIMERS; i++)
        {
            g_time.frame_timer[i] += g_time.delta;
        }
        for(i32 i = 0; i < MAX_FRAME_TIMERS; i++)
        {
            if (g_time.frame_timer[i] >= (1.0f/g_time.frame_rate[i]))
            {
                g_time.frame_timer[i] = 0;
            }
        }
    }
}