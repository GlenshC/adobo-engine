#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <ctime>
#include <algorithm>

#include "core/game.h"
#include "core/entity/ecs.h"
#include "core/platform.h"
#include "core/input.h"
#include "core/clock.h"

#include "core/math/collisions.h"

#include "renderer/sprite2D.h"
#include "renderer/shader.h"
#include "renderer/texture.h"

#include "util/debug_state.h"
#include "util/debug.h"

#include "../assets/bin.h"
#include "binassets/binasset_read.h"
#include "ggb/data_struct.h"

#include "imgui.h"
#include "core/editor.h"

static void win_resize(int width, int height);
static void mbtn_callback(int button, int action, int mods);
static void key_input(int key, int scancode, int action, int mods);

enum EntityType
{
    EntPlayer = 0
};


void update_player(const ecs::Entity2Dref &data)
{
    const f32 speed = 0.001f;
    if (glfwGetKey(plat::g_window(), GLFW_KEY_W))
    {
        data.position.y -= speed;
    }
    if (glfwGetKey(plat::g_window(), GLFW_KEY_S))
    {
        data.position.y += speed;
    }
    if (glfwGetKey(plat::g_window(), GLFW_KEY_A))
    {
        data.position.x -= speed;
    }
    if (glfwGetKey(plat::g_window(), GLFW_KEY_D))
    {
        data.position.x += speed;
    }
}

void ecs::update()
{
    adobo::AdoboScene *s;
    if ((s = editor::proj_active_scene()))
    {
        for (auto &e : s->m_entities)
        {
            if (e.m_id().type == EntPlayer)
            {
                update_player(e.m_id());
            }
        }
    }
}

namespace game {

    void init(void) 
    {
        // INITS
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        texture::set_onload_flipv(false);
        plat::set_vsync(true);
        plat::set_mousebtn_cb(mbtn_callback);
        plat::set_keyboard_cb(key_input);
        plat::set_framebuffer_cb(win_resize);
        editor::editor_init();

        // glAlphaFunc(GL_GREATER, 0.5);
        // glEnable(GL_ALPHA_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glEnable(GL_DEPTH_TEST);
    }
    
    void update(double dTime)
    {
        (void) dTime;
        ecs::update();

    }
    
    void render(void)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        glDepthMask(GL_FALSE);
        
        editor::editor_render();
        glDepthMask(GL_TRUE);
    }

    void shutdown(void)
    {

    }
    
    void gui(void)
    {
        static i32 fps_game = 0;

        if (clk::g_time.frame_timer[5] == 0)
        {
            fps_game = clk::g_fps.fps;

        }
        ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoMove;

        // Position at top-left
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);

        // Transparent background
        ImGui::SetNextWindowBgAlpha(0.35f); // 0.0 = fully transparent, 1.0 = solid

        if (ImGui::Begin("Debug Overlay", nullptr, window_flags))
        {
            ImGui::Text("Game  FPS: %d", fps_game);
            ImGui::Text("Mouse: %.2lf, %.2lf", plat::g_window.mpos.x, plat::g_window.mpos.y);
            ImGui::Text("Win: %d %d", plat::g_window.width, plat::g_window.height);
            ImGui::Text("ECS s,c: %zu %zu", ecs::g_entities.size, ecs::g_entities.capacity);
            ImGui::Text("S2D s,c: %zu %zu", renderer::g_sprites.size, renderer::g_sprites.capacity);
            ImGui::Text("TEX s,c: %zu %zu", texture::g_textures.size, texture::g_textures.capacity);
        }
        ImGui::End();

        editor::editor_gui();
    }

}
void win_resize(int width, int height)
{
    (void) width, (void) height;

}

void mbtn_callback(int button, int action, int mods)
{
    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // DEBUG_LOG("L_CLICK\n");
    }
}

void key_input(int key, int scancode, int action, int mods)
{
    (void)scancode, (void)mods, (void) action, (void) key;
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_S && (mods & GLFW_MOD_CONTROL))
        {
            DEBUG_LOG("SAVING\n");
            editor::editor_save();
        }

    }
}