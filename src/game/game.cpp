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
#include "res/graphics/shader.h"
#include "res/graphics/texture.h"

#include "util/debug_state.h"
#include "util/debug.h"

#include "../assets/bin.h"
#include "binassets/binasset_read.h"

#include "imgui.h"

struct GameState
{
    f32                *p_scale;
    f32                *p_rotation;
    f32                *p_position;
    i32                 score;
    i32                 bullets;
};

// enum OBJ
// {
//     CROSSHAIR
// };

static GameState g_state;
static shader::Shader           s_sprite_shader;
static mat4                     u_projection = GLM_MAT2_IDENTITY_INIT;

static texture::Texture         shoot, shoot_hud, shoot_stall;
static ggb::AssetData           assets;
double xpos, ypos;

static ecs::Entity2DGroup<9> wave[2];
static ecs::Entity2D         player;
static ecs::Entity2D         crosshair; 
static ecs::Entity2D         table;
static ecs::Entity2D         background;
static ecs::Entity2D         curtain[9];
static ecs::Entity2D         ducks[10];


static u32 fast_rand_seed = 2463534242; 
inline u32 fast_rand();
adobo::vec2f ball_nextpos();
void win_resize(int width, int height);

namespace game {
    static void key_input(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void toggle_cursor();
    // run once
    void init(void) 
    {
        glClearColor(0,0,0,1);
        glfwSetKeyCallback(plat::g_window(), key_input);
        plat::set_framebuffer_cb(win_resize);
        win_resize(0,0);
        fast_rand_seed = (u32)std::time(nullptr);

        renderer::init_sprites();
        texture::set_onload_flipv(false);
        plat::set_vsync(false);

        ggb::assets_load_bin(assets, "./assets/bin.glnsh");
        shader::create(s_sprite_shader, "./assets/shader/sprite.vert", "./assets/shader/sprite.frag");
        
        auto &atlas_shoot = assets.atlases[ATLAS::SHOOT];
        auto &atlas_shoot_hud = assets.atlases[ATLAS::SHOOT_HUD];
        auto &atlas_shoot_stall = assets.atlases[ATLAS::SHOOT_STALL];
        shoot = texture::loadAtlas2D(atlas_shoot, GL_RGBA);
        shoot_hud = texture::loadAtlas2D(atlas_shoot_hud, GL_RGBA);
        shoot_stall = texture::loadAtlas2D(atlas_shoot_stall, GL_RGBA);

        {
            auto data = ecs::create(player);
            data.position = {SCREEN_CENTER_X, SCREEN_HEIGHT * 0.94f};
            data.scale = {0.1f * SCREEN_WIDTH, 0.36f * SCREEN_HEIGHT};

            data.tex = shoot;
            data.tex_uv = shoot[SHOOT::RIFLE];

            g_state.p_scale = data.scale;
            g_state.p_rotation = data.rotation;
            g_state.p_position = data.position;
        }
        {
            const auto &data = ecs::create(crosshair);
            data.tex = shoot_hud;
            data.tex_uv = shoot_hud[SHOOT_HUD::CROSS_WHITE_L];
            data.scale = {100,100};
        }
        {
            wave[0](shoot_stall, shoot_stall[SHOOT_STALL::WATER2], 200, SCREEN_HEIGHT-150, 250, 380);
            wave[1](shoot_stall, shoot_stall[SHOOT_STALL::WATER2], 280, SCREEN_HEIGHT-120, 250, 380);
        }
        {
            ecs::create(table, shoot_stall, shoot_stall[SHOOT_STALL::BG_WOOD], SCREEN_CENTER_X, SCREEN_HEIGHT + 65, SCREEN_WIDTH * 2, 356);
            ecs::create(ducks[0], shoot, shoot[SHOOT::DUCK_OUT_TARGET_B], SCREEN_CENTER_X, SCREEN_CENTER_Y, 160, 160);
            ecs::create(background, shoot_stall, shoot_stall[SHOOT_STALL::BG_WOOD], SCREEN_CENTER_X, SCREEN_CENTER_Y, SCREEN_WIDTH, SCREEN_HEIGHT);
            ecs::create(curtain[0], shoot_stall, shoot_stall[SHOOT_STALL::CURTAIN], 0.057f * SCREEN_WIDTH, 0.445f * SCREEN_HEIGHT, SCREEN_WIDTH * 0.1316f, 0.914f*SCREEN_HEIGHT);
            ecs::create(curtain[1], shoot_stall, shoot_stall[SHOOT_STALL::CURTAIN_ROPE], 0, 0.448f * SCREEN_HEIGHT, 0.0391f * SCREEN_WIDTH, 0.03125f * SCREEN_HEIGHT);
        }
        // global

        // ggb::assets_data_free(assets);
    }

    void update(double dTime)
    {
        (void) dTime;
        if (plat::g_window.mstate)
        {
            plat::cursor_clamp();
        }
        auto &mpos = plat::g_window.mpos;
        crosshair().position = {(float)mpos.x, (float)mpos.y};


        // inverting
        if (mpos.x <= SCREEN_CENTER_X)
        {
            player().rotation.y = glm_rad(180.0f);
        }
        else
        {
            player().rotation.y = 0;
        }
            
        // player().position.y = (float)SCREEN_HEIGHT - 40.0f;
        player().position.x = (float)mpos.x;

        glm_mat4_identity(u_projection);
        glm_ortho(0.0f, (float)plat::g_window.width, (float)plat::g_window.height, 0, -200.0f, 200.0f, u_projection);

    }

    void render(void)
    {
        shader::bind(s_sprite_shader);
        shader::set_uniform_mat4(s_sprite_shader, "u_projection", u_projection);

        renderer::begin_sprites(s_sprite_shader);
        
        renderer::submit_sprites(background);
        renderer::submit_sprites(wave[0]);
        renderer::submit_sprites(ducks[0]);
        renderer::submit_sprites(wave[1]);
        renderer::submit_sprites(crosshair);
        renderer::submit_sprites(table);
        renderer::submit_sprites(player);
        renderer::submit_sprites(curtain[0]);
        renderer::submit_sprites(curtain[1]);
        
        renderer::end_sprites();
    }

    void shutdown(void)
    {

    }
    
    void gui(void)
    {
        static i32 fps_game = 0;
        static f32 rotation[3] = {0,0,32};
        
        if (clk::g_time.frame_timer[1] == 0)
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
            ImGui::Text("%d %d", plat::g_window.width, plat::g_window.height);
            ImGui::DragFloat2("Scale ", g_state.p_scale, 1.0f, -1000, 4000, "%.1f");
            ImGui::DragFloat2("Position", g_state.p_position, 1.0f, -1000, 4000, "%.1f");
            if (ImGui::DragFloat3("Rotation", rotation, 0.5f, -180, 180, "%.1f"))
            {
                g_state.p_rotation[0] = glm_rad(rotation[0]);
                g_state.p_rotation[1] = glm_rad(rotation[1]);
                g_state.p_rotation[2] = glm_rad(rotation[2]);
            }
            ImGui::Text("Objects");
            // bool datupdate[3]={};
            ImGui::DragFloat2("Curtain Position", curtain[1]().position, 1.0f, -100, 4000, "%.1f");
            ImGui::DragFloat2("Curtain Scale", curtain[1]().scale, 1.0f, -100, 4000, "%.1f");
        }
        ImGui::End();
    
    }

    void key_input(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        (void) window, (void) scancode, (void) mods;
        if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_ESCAPE)
            {
                toggle_cursor();
            }
        }
    }

    void toggle_cursor()
    {
        static adobo::vec2<f64> prev_mpos;
        if (plat::g_window.mstate)
        {
            prev_mpos.x = plat::g_window.mpos.x;
            prev_mpos.y = plat::g_window.mpos.y;
            plat::enable_cursor(true);
            plat::set_cursor_pos(prev_mpos.x, prev_mpos.y);
        }
        else
        {
            plat::enable_cursor(false);
        }
    }
}


void win_resize(int width, int height)
{
    (void) width, (void) height;
    {
        auto pdat = player();
        pdat.position = {SCREEN_CENTER_X, SCREEN_HEIGHT * 0.94f};
        pdat.scale = {0.1f * SCREEN_WIDTH, 0.36f * SCREEN_HEIGHT};
    }
    {
        auto cdat = curtain[1]();
        cdat.position = {0, 0.448f * SCREEN_HEIGHT};
        cdat.scale = {0.0391f * SCREEN_WIDTH, 0.03125f * SCREEN_HEIGHT};
    }
    {
        auto cdat = curtain[0]();
        cdat.position = {0.057f * SCREEN_WIDTH, 0.445f * SCREEN_HEIGHT};
        cdat.scale = {0.1316f * SCREEN_WIDTH, 0.914f * SCREEN_HEIGHT};
    }
    {
        auto bgdat = background();
        bgdat.position = {SCREEN_CENTER_X, SCREEN_CENTER_Y};
        bgdat.scale = {(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
    }
    {
        auto wave1 = wave[0]();
        wave1.position = {0.095f * SCREEN_WIDTH, 0.84f * SCREEN_HEIGHT};
        wave1.scale = {0.114f * SCREEN_WIDTH, 0.24f * SCREEN_HEIGHT};
        wave[0].update();
    }
    {
        auto wave2 = wave[1]();
        wave2.position = {0.05f * SCREEN_WIDTH, 0.86f * SCREEN_HEIGHT};
        wave2.scale = {0.114f * SCREEN_WIDTH, 0.24f * SCREEN_HEIGHT};
        wave[1].update();
    }
    {
        auto tabdat = table();
        tabdat.position = {0.98f * SCREEN_WIDTH, 1.11f * SCREEN_HEIGHT};
        tabdat.scale = {2.0f * SCREEN_WIDTH, 0.47f * SCREEN_HEIGHT};
    }
}


inline u32 fast_rand() 
{
    fast_rand_seed ^= fast_rand_seed << 13;
    fast_rand_seed ^= fast_rand_seed >> 17;
    fast_rand_seed ^= fast_rand_seed << 5;
    return (fast_rand_seed * 0x27d4eb2d);
}

adobo::vec2f ball_nextpos()
{
    u32 randnum = fast_rand();
    return {(float)((randnum % 13) * 80) + 80.0f, (float)((randnum % 9) * -80.0f)};
}
