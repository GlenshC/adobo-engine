#include "core/input.h"
#include "platform.h"
namespace input {
    i32 get_key(i32 glfw_key)
    {
        return glfwGetKey(plat::g_window.handle, glfw_key);
    }
}