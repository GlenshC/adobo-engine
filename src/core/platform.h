#pragma once

#include "types.h"
#define SCREEN_WIDTH       (plat::g_window.width)
#define SCREEN_HEIGHT      (plat::g_window.height)
#define SCREEN_CENTER_X    (SCREEN_WIDTH/2.0f)
#define SCREEN_CENTER_Y    (SCREEN_HEIGHT/2.0f)

namespace plat {
    typedef struct GLFWwindow *Window;

    struct WindowInfo{
        Window handle;
        int width;
        int height;

        Window& operator()(void)
        {
            return handle;
        }
    };

    extern WindowInfo g_window;

    i32     init(const char* title, int width, int height);
    void    shutdown(void);
    
    Window get_window(void);
    bool    should_close(void);
    f64     get_time(void);
    void    set_vsync(bool enabled);
    void    poll_events(void);
    void    swap_buffers(void);

    void    sleep(double delay);
}