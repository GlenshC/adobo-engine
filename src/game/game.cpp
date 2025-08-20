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
#include "ggb/data_struct.h"

#include "imgui.h"
#include <algorithm>

#define MAX_BULLET_DECAL 10
struct GameState
{
    f32                *p_scale = nullptr;
    f32                *p_rotation = nullptr;
    f32                *p_position = nullptr;
    i32                 score = 0;
    i32                 bullets = 0;
    bool                debug = true;
};

static GameState g_state;
static shader::Shader           s_sprite_shader;
static shader::Shader           s_debug_shader;
static mat4                     u_projection = GLM_MAT2_IDENTITY_INIT;

static texture::Texture         shoot, shoot_hud, shoot_stall;
static bsst::AssetData         &assets = bsst::g_assets;
double xpos, ypos;

static ecs::Entity2DGroup<9> wave[2];
static ecs::Entity2DGroup<4> curtain_curve;
static ecs::Entity2D         player;
static ecs::Entity2D         crosshair; 
static ecs::Entity2D         table;
static ecs::Entity2D         background;
static ecs::Entity2D         curtain[9];

static ggb::RingQueue<ecs::Entity2D, MAX_BULLET_DECAL> a_decals;
static ggb::SparseArr<ecs::Entity2D, 10> ducks;


static u32 fast_rand_seed = 2463534242; 
inline u32   fast_rand();
adobo::vec2f ball_nextpos();

static bool duck_check_hit();
static void win_resize(int width, int height);
static void mbtn_callback(int button, int action, int mods);
static void key_input(GLFWwindow* window, int key, int scancode, int action, int mods);
static void toggle_cursor();
namespace game {
    // run once
    void init(void) 
    {
        // INITS
        glClearColor(0,0,0,1);
        glfwSetKeyCallback(plat::g_window(), key_input);
        fast_rand_seed = (u32)std::time(nullptr);
        plat::set_framebuffer_cb(win_resize);
        plat::set_mousebtn_cb(mbtn_callback);
        renderer::init_sprites();
        texture::set_onload_flipv(false);
        plat::set_vsync(true);
        
        bsst::assets_free_on_load(true);
        bsst::assets_load_bin("bin.glnsh");
        bsst::assets_upload_atlases(assets);
        shader::create(s_sprite_shader, assets.shaders[SHADER::V_SPRITE], assets.shaders[SHADER::F_SPRITE]);
        shader::create(s_debug_shader, "./assets/shader/debug.vert", "./assets/shader/debug.frag");
        
        shoot = assets.atlases[ATLAS::SHOOT];
        shoot_hud = assets.atlases[ATLAS::SHOOT_HUD];
        shoot_stall = assets.atlases[ATLAS::SHOOT_STALL];

        {
            auto data = ecs::create(player);
            data.position = {SCREEN_CENTER_X, SCREEN_HEIGHT * 0.94f};
            data.scale = {0.1f * SCREEN_WIDTH, 0.36f * SCREEN_HEIGHT};

            data.tex = assets.atlases[ATLAS::SHOOT];
            data.tex_uv = assets.atlases[ATLAS::SHOOT][SHOOT::RIFLE];
            // auto a = assets.atlases[ATLAS::SHOOT][SHOOT::RIFLE];

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
            ecs::create(wave[0], shoot_stall, shoot_stall[SHOOT_STALL::WATER2], 200, SCREEN_HEIGHT-150, 250, 380);
            ecs::create(wave[1], shoot_stall, shoot_stall[SHOOT_STALL::WATER2], 280, SCREEN_HEIGHT-120, 250, 380);
        }
        {

            for (size_t i = 0; i < ducks.capacity; i++)
            {
                ecs::create(ducks[i], shoot, shoot[SHOOT::DUCK_OUT_TARGET_B], i * 200 + 200, SCREEN_CENTER_Y, 160, 160);
                ducks.size++;
            }
            ecs::create(table, shoot_stall, shoot_stall[SHOOT_STALL::BG_WOOD]);
            ecs::create(background, shoot_stall, shoot_stall[SHOOT_STALL::BG_WOOD]);
            ecs::create(curtain[0], shoot_stall, shoot_stall[SHOOT_STALL::CURTAIN]); // curtain_l
            ecs::create(curtain[1], shoot_stall, shoot_stall[SHOOT_STALL::CURTAIN_ROPE] ); // rope_l
            ecs::create(curtain[2], shoot_stall, shoot_stall[SHOOT_STALL::CURTAIN]); // curtain_r
            ecs::create(curtain[3], shoot_stall, shoot_stall[SHOOT_STALL::CURTAIN_ROPE] ); // rope_r
            for (int i = 4; i <= 8; i++)
                ecs::create(curtain[i], shoot_stall, shoot_stall[SHOOT_STALL::CURTAIN_TOP]);
            ecs::create(curtain_curve, shoot_stall, shoot_stall[SHOOT_STALL::CURTAIN_STRAIGHT], 0, 0, 0.1316f * SCREEN_WIDTH, 0.3f * SCREEN_HEIGHT); // rope_r

            for (int i = 0; i < MAX_BULLET_DECAL; i++)
            {
                auto bullet_decal = ecs::create(a_decals[i], shoot, shoot[SHOOT::SHOT_G_S]);
                bullet_decal.scale = {32, 32};
            }
        }

        win_resize(SCREEN_WIDTH, SCREEN_HEIGHT);

        glm_mat4_identity(u_projection);
        glm_ortho(0.0f, (float)plat::g_window.width, (float)plat::g_window.height, 0, -1.0f, 1.0f, u_projection);
    }

    #define GUN_MPOSX_OFFSET ((f32)160)
    void update(double dTime)
    {
        (void) dTime;
        static f32 gun_offset = GUN_MPOSX_OFFSET;
        if (plat::g_window.mstate)
        {
            plat::cursor_clamp();
        }
        auto &mpos = plat::g_window.raw_mpos;
        crosshair().position = {(float)mpos.x, (float)mpos.y + 10.0f};


        // inverting
        if (mpos.x <= SCREEN_CENTER_X)
        {
            player().rotation.y = glm_rad(180.0f);
            gun_offset = -GUN_MPOSX_OFFSET;
        }
        else
        {
            player().rotation.y = 0;
            gun_offset = GUN_MPOSX_OFFSET;
        }
            
        // player().position.y = (float)SCREEN_HEIGHT - 40.0f;
        f32 temp_pos = (float)mpos.x + gun_offset;
        if (temp_pos > SCREEN_WIDTH)
            temp_pos = SCREEN_WIDTH;
        else if (temp_pos < 0)
            temp_pos = 0;
        player().position.x = temp_pos;

    }

    void render(void)
    {
        shader::bind(s_sprite_shader);
        shader::set_uniform_mat4(s_sprite_shader, "u_projection", u_projection);

        renderer::begin_sprites(s_sprite_shader);
        
        renderer::submit_sprites(background);
        renderer::submit_sprites(wave[0]);
        for (size_t i = 0; i < ducks.size; i++)
        {
            renderer::submit_sprites(ducks[i]);
        }
        renderer::submit_sprites(wave[1]);
        
        for (int i = a_decals.f_index, j = 0; j < a_decals.size; i++, j++)
        {
            renderer::submit_sprites(a_decals[i % a_decals.capacity]);
        }
        
        renderer::submit_sprites(crosshair);
        renderer::submit_sprites(table);
        renderer::submit_sprites(curtain[4]);
        renderer::submit_sprites(curtain[8]);
        renderer::submit_sprites(curtain[5]);
        renderer::submit_sprites(curtain[7]);
        renderer::submit_sprites(curtain[6]);
        for (int i =0; i < 4; i++)
        {
            renderer::submit_sprites(curtain[i]);
        }
        renderer::submit_sprites(curtain_curve);
        renderer::submit_sprites(player);
        
        renderer::end_sprites();
        

        if (g_state.debug)
        {

        }
    }

    void shutdown(void)
    {

    }
    
    void gui(void)
    {
        static int index = 0;
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
            bool datupdate[3]={};
            ImGui::Text("Game  FPS: %d", fps_game);
            ImGui::Text("Mouse: %.2lf, %.2lf", plat::g_window.mpos.x, plat::g_window.mpos.y);
            ImGui::Text("Win: %d %d", plat::g_window.width, plat::g_window.height);
            ImGui::Text("Score: %d", g_state.score);
            ImGui::Text("Decals s,f,b: %d %d %d", a_decals.size, a_decals.f_index, a_decals.b_index);
            ImGui::Text("Ducks s,c: %zu %zu", ducks.size, ducks.capacity);
            ImGui::Text("ECS s,c: %d %d", ecs::g_entities.size(), ecs::g_entities.capacity);
            ImGui::DragFloat2("Scale ", g_state.p_scale, 1.0f, -1000, 4000, "%.1f");
            ImGui::DragFloat2("Position", g_state.p_position, 1.0f, -1000, 4000, "%.1f");
            if (ImGui::DragFloat3("Rotation", rotation, 0.5f, -180, 180, "%.1f"))
            {
                g_state.p_rotation[0] = glm_rad(rotation[0]);
                g_state.p_rotation[1] = glm_rad(rotation[1]);
                g_state.p_rotation[2] = glm_rad(rotation[2]);
            }
            ImGui::Text("Objects");

            ImGui::InputInt("Duck Index: ", &index);
            index = std::clamp(index, 0, 9);
            datupdate[0] |= ImGui::DragFloat2("Duck Position", ducks[index]().position, 1.0f, -100, 4000, "%.1f");
            datupdate[0] |= ImGui::DragFloat2("Duck Scale", ducks[index]().scale, 1.0f, -100, 4000, "%.1f");
        }
        ImGui::End();
    
    }


}


void win_resize(int width, int height)
{
    (void) width, (void) height;
    glm_mat4_identity(u_projection);
    glm_ortho(0.0f, (float)plat::g_window.width, (float)plat::g_window.height, 0, -1.0f, 1.0f, u_projection);

    {
        auto pdat = player();
        pdat.position = {SCREEN_CENTER_X, SCREEN_HEIGHT * 0.94f};
        pdat.scale = {0.1f * SCREEN_WIDTH, 0.36f * SCREEN_HEIGHT};
    }
    {
        auto curt1 = curtain[0]();
        curt1.position = {0.057f * SCREEN_WIDTH, 0.445f * SCREEN_HEIGHT};
        curt1.scale = {0.1316f * SCREEN_WIDTH, 0.914f * SCREEN_HEIGHT};

        auto rope1  = curtain[1]();
        rope1.position = {0, 0.448f * SCREEN_HEIGHT};
        rope1.scale = {0.0391f * SCREEN_WIDTH, 0.03125f * SCREEN_HEIGHT};

        auto curt2 = curtain[2]();
        curt2.position = {0.94f * SCREEN_WIDTH, 0.445f * SCREEN_HEIGHT};
        curt2.scale = {0.1316f * SCREEN_WIDTH, 0.914f * SCREEN_HEIGHT};
        curt2.rotation.y = glm_rad(180);

        auto rope2  = curtain[3]();
        rope2.position = {0.991f * SCREEN_WIDTH, 0.448f * SCREEN_HEIGHT};
        rope2.scale = {0.0391f * SCREEN_WIDTH, 0.03125f * SCREEN_HEIGHT};

        auto curts = curtain_curve();
        curts.position = {0, 0.06125f * SCREEN_HEIGHT};
        curts.scale = {0.3316f * SCREEN_WIDTH, 0.158f * SCREEN_HEIGHT};
        curtain_curve.update();
    }
    {
        for (int i = 4; i <= 8; i++)
        {
            auto curt = curtain[i]();
            curt.position =  {0.117f * SCREEN_WIDTH + (i-4) * 0.242f * SCREEN_WIDTH * 0.80f, 0.16f * SCREEN_HEIGHT};
            curt.scale  = {0.242f * SCREEN_WIDTH, 0.1168f * SCREEN_HEIGHT};
        }
    }
    {
        auto bgdat = background();
        bgdat.position = {SCREEN_CENTER_X, SCREEN_CENTER_Y};
        bgdat.scale = {(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
    }
    {
        auto wave1 = wave[0]();
        wave1.position = {0.095f * SCREEN_WIDTH, 0.82f * SCREEN_HEIGHT};
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

void mbtn_callback(int button, int action, int mods)
{
    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        duck_check_hit();
        if (a_decals.size >= a_decals.capacity)
        {
            a_decals.pop();
        }
        auto &decal = a_decals.push();
        decal().position = {(float)plat::g_window.mpos.x, (float)plat::g_window.mpos.y + 10.0f };
        DEBUG_LOG("L_CLICK\n");
    }
}

void key_input(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)window, (void)scancode, (void)mods;
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ESCAPE)
        {
            toggle_cursor();
        }
        if (key == GLFW_KEY_ENTER)
        {
            ducks.push();
        }
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

static bool duck_check_hit()
{
    return true;
}