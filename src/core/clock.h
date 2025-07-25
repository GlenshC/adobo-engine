#pragma once
#include "types.h"
#include "core/init.h"
#include <stdint.h>


namespace clk {
    struct TimeState
    {
        f64     last;    
        f64     start;
        f64     now;     
        f64     delta; 
        // f64     elapsed; 
    };

    struct FPSState
    {
        f64     timer;
        i32     fps;
        i32     count;
    };

    extern TimeState g_time;
    extern FPSState g_fps;

    void    init(void);
    void    update(void);
    // void    tick(void);

    inline f64
    get_dtime(void) 
    {
        return g_time.delta;
    }
    
    inline f64
    get_time(void) 
    {
        return g_time.now;
    }
    
    inline i32
    get_fps(void)
    {
        return g_fps.fps;
    }
}