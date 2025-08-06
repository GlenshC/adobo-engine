#include <glad/gl.h>
#include "core/game.h"
#include "core/entity/ecs.h"
#include "core/platform.h"
#include "renderer/sprite2D.h"
#include "resource/graphics/shader.h"
#include "resource/graphics/texture.h"

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
        platform::set_vsync(true);

        shader::create(s_sprite_shader, "./assets/shader/sprite.vert", "./assets/shader/sprite.frag");
        atlas = texture::loadAtlas2D("./assets/sprites/debug.png", GL_RGBA, 32, 32);

        ecs::Entity2Dref block_data = ecs::create_entity(block_ent);
        block_data.tex_id = atlas;
        block_data.frames = {2, 2, 0};
        block_data.transform = { .position = {SCREEN_CENTER_X, SCREEN_CENTER_Y}, .scale = {200, 200}};
    }
    
    void update(double dTime)
    {
        (void) dTime;
        glm_mat4_identity(u_projection);
        glm_ortho(0.0f, (float)platform::g_window.width, (float)platform::g_window.height, 0, -1.0f, 1.0f, u_projection);
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