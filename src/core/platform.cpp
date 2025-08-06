#include <stdio.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "./platform.h"

namespace plat 
{
    WindowInfo g_window; 
    Window win_handle;
    
    void fbuffer_callback_default(GLFWwindow *window, int w, int h);

    int init(const char *title, int width, int height)
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

        // Create a windowed mode window and OpenGL context
        win_handle = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!win_handle)
        {
            fprintf(stderr, "Failed to create GLFW window\n");
            glfwTerminate();
            return -1;
        }
        
        // Make the OpenGL context current
        glfwMakeContextCurrent(win_handle);

        // Load OpenGL functions via GLAD
        if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
        {
            fprintf(stderr, "Failed to initialize GLAD\n");
            glfwDestroyWindow(win_handle);
            glfwTerminate();
            return -1;
        }

        // Print OpenGL version
        printf("OpenGL %s\n", glGetString(GL_VERSION));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set the viewport
        g_window.handle = win_handle;
        g_window.width = width;
        g_window.height = height;
        glfwSetFramebufferSizeCallback(win_handle, fbuffer_callback_default);
        

        return 0;
    }

    Window get_window()
    {
        return (Window)win_handle;
    }

    void shutdown(void)
    {
        glfwDestroyWindow(win_handle);
        glfwTerminate();
    }
    
    bool should_close(void)
    {
        return glfwWindowShouldClose(win_handle);
    }

    void poll_events(void)
    {
        glfwPollEvents();
    }

    void swap_buffers(void)
    {
        glfwSwapBuffers(win_handle);
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

    void fbuffer_callback_default(GLFWwindow *window, int w, int h)
    {
        (void) window;
        glViewport(0,0,w,h);
        g_window.width = w;
        g_window.height = h;
    }
}