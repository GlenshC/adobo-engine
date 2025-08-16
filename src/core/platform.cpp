#include <stdio.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "./platform.h"

namespace plat 
{
    WindowInfo g_window; 
    
    void fbuffer_callback_default(GLFWwindow *window, int w, int h);
    void window_focus_callback(GLFWwindow* window, int focused);

    int init(const char *title)
    {
        // Initialize GLFW
        if (!glfwInit())
        {
            fprintf(stderr, "Failed to initialize GLFW\n");
            return -1;
        }
        
        // Request OpenGL 3.3 Core Profile
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        int width = 1200;
        int height = 800;
        
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        width = mode->width;
        height = mode->height;

        // Create a windowed mode window and OpenGL context
        GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!window)
        {
            fprintf(stderr, "Failed to create GLFW window\n");
            glfwTerminate();
            return -1;
        }
        glfwSetWindowPos(window, 0, 0);
        
        // Make the OpenGL context current
        glfwMakeContextCurrent(window);
        glfwShowWindow(window);
        glfwFocusWindow(window);
        glfwPollEvents();
        
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
        return 0;
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

    void swap_buffers(void)
    {
        glfwSwapBuffers(g_window.handle);
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
        glfwGetCursorPos(g_window.handle, &g_window.mpos.x, &g_window.mpos.y);
    }

    void fbuffer_callback_default(GLFWwindow *window, int w, int h)
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

    void set_framebuffer_cb(FBUFFERCBFUNC *funcptr)
    {
        g_window.fbuf_cbfunc = funcptr;
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
        f64 &x = g_window.mpos.x;
        f64 &y = g_window.mpos.y;
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
    }

    void window_focus_callback(GLFWwindow* window, int focused) {
        (void) window;
        g_window.is_focused = (focused == GLFW_TRUE);
    }
}