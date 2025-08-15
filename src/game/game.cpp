#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <ctime>
#include <algorithm>

#include "core/game.h"
#include "core/entity/ecs.h"
#include "core/platform.h"
#include "core/input.h"

#include "core/math/collisions.h"

#include "renderer/sprite2D.h"
#include "res/graphics/shader.h"
#include "res/graphics/texture.h"

#include "util/debug_state.h"
#include "util/debug.h"
#include "assets/bin.h"


#include "imgbin.h"
static shader::Shader           s_sprite_shader;
static mat4                     u_projection = GLM_MAT2_IDENTITY_INIT;
static texture::Texture         atlas;
static texture::Texture         metrix;

#define BALL_COUNT 5
static ecs::Entity2D    balls[BALL_COUNT];
static ecs::Entity2D    player;
double xpos, ypos;

static u32 fast_rand_seed = 2463534242; // you can set this to time(NULL) for randomness

inline u32 fast_rand() {
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

namespace game {
    // run once
    void init(void) 
    {
        glClearColor(0,0,0,1);
        fast_rand_seed = (u32)std::time(nullptr);

        renderer::init_sprites();
        texture::set_onload_flipv(false);
        plat::set_vsync(false);

        ggb::AssetData assets;
        ggb::assets_load_bin(assets, "./assets/bin.glnsh");
        
        // shader::create(s_sprite_shader, assets.shaders, assets.shaders);
        // atlas  = texture::loadAtlas2D(assets.imgs["t_ball"], GL_RGBA, 64, 64);
        // metrix = texture::loadAtlas2D(assets.imgs["t_metrix"], GL_RGBA, 64, 64);


        shader::create(s_sprite_shader, "./assets/shader/sprite.vert", "./assets/shader/sprite.frag");
        atlas = texture::loadAtlas2D("./assets/sprites/texture.png", GL_RGBA, 64, 64);
        metrix = texture::loadAtlas2D("./assets/sprites/metrix_tex.png", GL_RGBA , 64, 64);
        
        for (i32 i = 0; i < BALL_COUNT; i++)
        {
            ecs::Entity2Dref data = ecs::create_entity(balls[i]);
            data.position = ball_nextpos();
            data.scale = {64,64};
            data.tex = atlas;
            data.radius = 32;
        }

        ecs::Entity2Dref data = ecs::create_entity(player);
        data.position = {SCREEN_CENTER_X, (float)SCREEN_HEIGHT - 32.0f};
        data.scale = {90, 16};
        data.tex = metrix;
        data.frames = {1,1,0,0};

        ggb::assets_data_free(assets);
    }

    void update(double dTime)
    {
        (void) dTime;
        glfwGetCursorPos(plat::g_window(), &xpos, &ypos);
        f32 player_rx = player().scale.x * 0.5f;
        if (xpos < player_rx + 10.0f)
            xpos = player_rx + 10.0f;
        if (xpos >= SCREEN_WIDTH - (player_rx + 10.0f))
        {
            xpos = SCREEN_WIDTH - (player_rx + 10.0f);
        }
        player().position.x = (float)xpos;



        for (i32 i = 0; i < BALL_COUNT; i++)
        {
            ecs::Entity2Dref ball_data = balls[i]();
            ball_data.position.y += 200.0f * (float)dTime;

            f32 ball_top = ball_data.position.y - ball_data.radius;

            if (ball_top >= SCREEN_HEIGHT)
            {
                ball_data.position = ball_nextpos();
            }
            else if (math::collision_circleAABB(ball_data.position, ball_data.radius, ecs::get_aabb(player)))
            {
                ball_data.position = ball_nextpos();
            }
        }
        glm_mat4_identity(u_projection);
        glm_ortho(0.0f, (float)plat::g_window.width, (float)plat::g_window.height, 0, -1.0f, 1.0f, u_projection);

        ecs::update_all(dTime);
    }

    void render(void)
    {
        shader::bind(s_sprite_shader);
        shader::set_uniform_mat4(s_sprite_shader, "u_projection", u_projection);

        renderer::begin_sprites(s_sprite_shader);
        
        renderer::submit_sprites(player);
        for (i32 i = 0; i < BALL_COUNT; i++)
        {
            renderer::submit_sprites(balls[i]);
        }
        
        renderer::end_sprites();
    }
    void shutdown(void)
    {

    }
}
