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
#include "game/editor.h"

static void win_resize(int width, int height);
static void mbtn_callback(int button, int action, int mods);
static void key_input(GLFWwindow* window, int key, int scancode, int action, int mods);

static ecs::Entity2D         player;
static shader::Shader s_sprite_shader;
static texture::Texture         shoot, shoot_hud, shoot_stall;
static mat4                     u_projection = GLM_MAT2_IDENTITY_INIT;
namespace game {

    void init(void) 
    {
        // INITS
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        texture::set_onload_flipv(false);
        plat::set_vsync(true);
        glfwSetKeyCallback(plat::g_window(), key_input);
        plat::set_mousebtn_cb(mbtn_callback);
        plat::set_framebuffer_cb(win_resize);
        // editor::editor_init();

        bsst::assets_free_on_load(true);
        bsst::assets_load_bin("./assets/bin.glnsh");
        bsst::assets_upload_atlases();

        auto &assets = bsst::g_assets;
        shader::create(s_sprite_shader, "./assets/shader/sprite.vert", "./assets/shader/sprite.frag");

        shoot = assets.atlases[BSST_ATLAS::SHOOT];
        shoot_hud = assets.atlases[BSST_ATLAS::SHOOT_HUD];
        shoot_stall = assets.atlases[BSST_ATLAS::SHOOT_STALL];

        {
            auto data = ecs::create(player);
            data.position = {0.5f, 0.94f};
            data.scale = {0.1f, 0.36f};

            data.tex = assets.atlases[BSST_ATLAS::SHOOT];
            data.tex_uv = assets.atlases[BSST_ATLAS::SHOOT][BSST_SHOOT::RIFLE];
        }
        
        glm_mat4_identity(u_projection);
        glm_ortho(0.0f, 1.0f, 1.0f, 0, -1.0f, 1.0f, u_projection);

    }

    void update(double dTime)
    {
        (void) dTime;
    }

    void render(void)
    {
        shader::bind(s_sprite_shader);
        shader::set_uniform_mat4(s_sprite_shader, "u_projection", u_projection);
        
        renderer::begin_sprites(s_sprite_shader);
        renderer::submit_sprites(player);
        renderer::end_sprites();
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

        // editor::editor_gui();
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

void key_input(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)window, (void)scancode, (void)mods, (void) action, (void) key;
    if (action == GLFW_PRESS)
    {

    }
}