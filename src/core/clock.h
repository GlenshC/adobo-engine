#pragma once
#include "types.h"
#include "core/init.h"
#include <stdint.h>


namespace clk {
    /* CONSTANTS */
    const i32 MAX_FRAME_TIMERS = 8;

    /* TYPES */
    struct TimeState;
    struct FPSState;

    /* GLOBALS */
    extern TimeState g_time;
    extern FPSState g_fps;

    /* FUNCTIONS */
    void    init(void);
    void    update(void);
    inline f64 get_dtime(void);
    inline f64 get_time(void);  
    inline i32 get_fps(void);   

    /* TYPE DEFS */
    struct TimeState
    {
        f64     last  = 0;    
        f64     start = 0;
        f64     now   = 0;     
        f64     delta = 0;
        f32     frame_timer[MAX_FRAME_TIMERS] = {};// tracks for frame_rate
        f32     frame_rate[MAX_FRAME_TIMERS] = {2, 4, 6, 8, 10, 12, 20, 24}; // 4hz 30hz etc
        // f64     elapsed; 
    };

    struct FPSState
    {
        f64     timer;
        i32     fps;
        i32     count;
    };

    /* FUNC DEFS */
    inline f64 get_dtime(void) { return g_time.delta; }
    inline f64 get_time(void)  { return g_time.now; }
    inline i32 get_fps(void)   { return g_fps.fps; }
}