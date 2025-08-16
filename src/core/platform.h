#pragma once

#include "types.h"
#include "GLFW/glfw3.h"

#define SCREEN_WIDTH       (plat::g_window.width)
#define SCREEN_HEIGHT      (plat::g_window.height)
#define SCREEN_CENTER_X    (SCREEN_WIDTH/2.0f)
#define SCREEN_CENTER_Y    (SCREEN_HEIGHT/2.0f)

namespace plat {
    typedef struct GLFWwindow *Window;
    typedef void FBUFFERCBFUNC(int width, int height);

    enum CURSOR_STATE {
        ADOBO_CURSOR_NORMAL,
        ADOBO_CURSOR_DISABLED,
    };

    struct WindowInfo{
        GLFWwindow    *handle    = nullptr;
        FBUFFERCBFUNC *fbuf_cbfunc = nullptr;
        
        adobo::vec2<f64> mpos;
        CURSOR_STATE   mstate;
        int width;
        int height;
        bool is_focused = 1;
        
        GLFWwindow *operator()(void)
        {
            return handle;
        }
    };

    extern WindowInfo g_window;

    i32     init(const char* title);
    void    update();
    void    shutdown(void);
    
    Window  get_window(void);
    bool    should_close(void);
    void    swap_buffers(void);
    void    poll_events(void);

    f64     get_time(void);
    void    set_vsync(bool enabled);
    void    set_framebuffer_cb(FBUFFERCBFUNC *funcptr);

    void    sleep(double delay);
    void    cursor_clamp();
    void    enable_cursor(bool enabled);
    void    set_cursor_pos(f64 x, f64 y);
}