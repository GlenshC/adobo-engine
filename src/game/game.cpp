#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "core/game.h"
#include "core/entity/ecs.h"
#include "core/platform.h"
#include "core/input.h"

#include "renderer/sprite2D.h"
#include "resource/graphics/shader.h"
#include "resource/graphics/texture.h"

#include "util/debug_state.h"
#include "util/debug.h"

static shader::Shader           s_sprite_shader;
static mat4                     u_projection = GLM_MAT2_IDENTITY_INIT;
static texture::Texture         atlas;

static ecs::Entity2D            block_ent;

#define BLOCK_SIZE  (SCREEN_HEIGHT/24.0f)

namespace game {
    // run once
    void init(void) 
    {
        glEnable(GL_DEPTH_TEST);

        renderer::init_sprites();
        texture::set_onload_flipv(false);
        plat::set_vsync(true);

        shader::create(s_sprite_shader, "./assets/shader/sprite.vert", "./assets/shader/sprite.frag");
        atlas = texture::loadAtlas2D("./assets/sprites/debug.png", GL_RGBA, 32, 32);

        ecs::Entity2Dref block_data = ecs::create_entity(block_ent);
        block_data.tex_id = atlas;
        block_data.frames = {0, 0, 5, 0};
        block_data.transform = { .position = {SCREEN_CENTER_X, SCREEN_CENTER_Y}, .scale = {200, 200}};
    }
    
    void update(double dTime)
    {
        f32 speed = 500 * dTime;
        ecs::Entity2Dref block_data = ecs::g_entities(block_ent);
        block_data.velocity.x = 0;
        block_data.velocity.y = 0;

        vec2 velocity = {};
        if (input::get_key(GLFW_KEY_W))
        {
            velocity[1] -= 1;
        }
        if (input::get_key(GLFW_KEY_S))
        {
            velocity[1] += 1;
        }
        if (input::get_key(GLFW_KEY_A))
        {
            velocity[0] -= 1;
        }
        if (input::get_key(GLFW_KEY_D))
        {
            velocity[0] += 1;
        }
        if (input::get_key(GLFW_KEY_EQUAL))
        {
            if (block_data.frames[3] < 8)
            {
                block_data.frames[3]++;
                DEBUG_LOG("%hu\n", block_data.frames[3]);
            }
        }
        if (input::get_key(GLFW_KEY_MINUS))
        {
            if (block_data.frames[3] != 0)
            {
                block_data.frames[3]--;
            }
        }



        if (velocity[0] || velocity[1])
        {
            glm_vec2_normalize(velocity);
            velocity[0] *= speed;
            velocity[1] *= speed;
            block_data.velocity.x = velocity[0]; 
            block_data.velocity.y = velocity[1];
        }
        dbg::g_dbg.speed = sqrtf(velocity[0] * velocity[0] + velocity[1] * velocity[1]);

        glm_mat4_identity(u_projection);
        glm_ortho(0.0f, (float)plat::g_window.width, (float)plat::g_window.height, 0, -1.0f, 1.0f, u_projection);

        ecs::update_all();
    }

    void render(void)
    {
        shader::bind(s_sprite_shader);
        shader::set_uniform_mat4(s_sprite_shader, "u_projection", u_projection);

        renderer::begin_sprites(s_sprite_shader);
        renderer::submit_sprites(block_ent);
        renderer::end_sprites();
    }
    void shutdown(void)
    {

    }
}