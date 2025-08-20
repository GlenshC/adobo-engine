#include <stdio.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#ifdef _WIN32
// #include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#include "./platform.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <windows.h>

void setBorderlessFullscreen(GLFWwindow* window) {
    HWND hwnd = glfwGetWin32Window(window);

    // Strip window decorations (title bar, borders, etc.)
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
    SetWindowLong(hwnd, GWL_STYLE, style);

    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

    // Resize to monitor bounds
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi =  {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(monitor, &mi);

    SetWindowPos(hwnd, HWND_TOP,
        mi.rcMonitor.left,
        mi.rcMonitor.top,
        mi.rcMonitor.right - mi.rcMonitor.left,
        mi.rcMonitor.bottom - mi.rcMonitor.top,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
}


namespace plat 
{
    WindowInfo g_window; 
    
    static void fbuffer_callback_default(GLFWwindow *window, int w, int h);
    static void mousebtn_callback_default(GLFWwindow* window, int button, int action, int mods);
    static void window_focus_callback(GLFWwindow* window, int focused);

    int init(const char *title)
    {
        // Initialize GLFW
        if (!glfwInit())
        {
            fprintf(stderr, "Failed to initialize GLFW\n");
            return -1;
        }
        
        // Request OpenGL 3.3 Core Profile
        int width = 1200;
        int height = 800;
        
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        width = mode->width;
        height = mode->height;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

        GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, title, NULL, NULL);
        if (!window)
        {
            fprintf(stderr, "Failed to create GLFW window\n");
            glfwTerminate();
            return -1;
        }

        glfwSetWindowPos(window, 0, 0);

        HWND hwnd = glfwGetWin32Window(window);
        SetForegroundWindow(hwnd);
        ShowWindow(hwnd, SW_SHOW);
        SetFocus(hwnd);

        glfwMakeContextCurrent(window);

        // Load OpenGL functions via GLAD
        if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
        {
            fprintf(stderr, "Failed to initialize GLAD\n");
            glfwDestroyWindow(g_window.handle);
            glfwTerminate();
            return -1;
        }
        
        // Print OpenGL version
        printf("OpenGL %s\n", glGetString(GL_VERSION));
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        
        // Set the viewport
        g_window.handle = window;
        g_window.width = width;
        g_window.height = height;

        glfwSetFramebufferSizeCallback(g_window.handle, fbuffer_callback_default);
        glfwSetWindowFocusCallback(window, window_focus_callback);
        glfwSetMouseButtonCallback(window, mousebtn_callback_default);
        return 0;
    }

    void swap_buffers(void)
    {
        glfwSwapBuffers(g_window.handle);
    }

    Window get_window()
    {
        return (Window)g_window.handle;
    }

    void shutdown(void)
    {
        glfwDestroyWindow(g_window.handle);
        glfwTerminate();
    }
    
    bool should_close(void)
    {
        return glfwWindowShouldClose(g_window.handle);
    }

    void poll_events(void)
    {
        glfwPollEvents();
    }


    void set_vsync(bool enabled)
    {
        glfwSwapInterval(enabled);
    }

    double get_time()
    {
        return glfwGetTime();
    }

    void sleep(double delay)
    {
        glfwWaitEventsTimeout(delay);
    }

    void update()
    {
        glfwGetCursorPos(g_window.handle, &g_window.raw_mpos.x, &g_window.raw_mpos.y);
        g_window.mpos = {g_window.raw_mpos.x / SCREEN_WIDTH, g_window.raw_mpos.y / SCREEN_HEIGHT};
    }

    static void fbuffer_callback_default(GLFWwindow *window, int w, int h)
    {
        (void) window;
        glViewport(0,0,w,h);
        g_window.width = w;
        g_window.height = h;
        if (g_window.fbuf_cbfunc)
        {
            g_window.fbuf_cbfunc(w,h);
        }
    }

    static void mousebtn_callback_default(GLFWwindow* window, int button, int action, int mods)
    {
        (void)window;
        if (g_window.mbtn_cbfunc)
        {
            g_window.mbtn_cbfunc(button, action, mods);
        }
    }

    void set_framebuffer_cb(FBUFFERCBFUNC *funcptr)
    {
        g_window.fbuf_cbfunc = funcptr;
    }

    void set_mousebtn_cb(MOUSEBTNCBFUNC *funcptr)
    {
        g_window.mbtn_cbfunc = funcptr;
    }

    void set_cursor_pos(f64 x, f64 y)
    {
        glfwSetCursorPos(g_window.handle, x, y);
    }


    void enable_cursor(bool enabled)
    {
        if (enabled)
        {
            glfwSetInputMode(g_window.handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            g_window.mstate = ADOBO_CURSOR_NORMAL;
            return;
        }
        glfwSetInputMode(g_window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        g_window.mstate = ADOBO_CURSOR_DISABLED;
    }

    void cursor_clamp()
    {
        f64 &x = g_window.raw_mpos.x;
        f64 &y = g_window.raw_mpos.y;
        if (x > g_window.width)
        {
            x = g_window.width;
        }      
        else if (x < 0) 
        {
            x = 0;
        }
        
        if (y > g_window.height)
        {
            y = g_window.height;
        }
        else if (y < 0) 
        {
            y = 0;
        }

        glfwSetCursorPos(g_window(), x, y);
        g_window.mpos = {x / SCREEN_WIDTH, y / SCREEN_HEIGHT};
    }

    static void window_focus_callback(GLFWwindow *window, int focused)
    {
        (void)window;
        g_window.is_focused = (focused == GLFW_TRUE);
    }
}