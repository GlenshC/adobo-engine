#include <glad/gl.h>

#include "core/app.h"
#include "core/platform.h"
#include "core/game.h"
#include "core/clock.h"
#include "core/gui.h"

#include "renderer/renderer.h"
#include "res/graphics/shader.h"

#include "util/debug.h"
#include <thread>

namespace core
{
    void main(const char *title)
    {
        plat::init(title);
        glClearColor(0.2f , 0.3f, 0.3f, 1.0f);
        
        clk::init();
        renderer::init();
        game::init();
        gui::init();
        
        while (!plat::should_close()) {
            
            if (!plat::g_window.is_focused)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
            
            clk::update();
            plat::update();
            // input::update();
            game::update(clk::g_time.delta);
            
            //test
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
            
            game::render();
            gui::render();
           
            glFinish();
            plat::swap_buffers();
            plat::poll_events();
        }
        game::shutdown();
        gui::shutdown();
        // renderer::shutdown();
        plat::shutdown();
    }
}

