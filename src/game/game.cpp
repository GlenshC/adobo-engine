#include <glad/gl.h>
#include "core/game.h"
#include "core/platform.h"
#include "resource/atlas.h"
#include "renderer/shader.h"
#include "renderer/r_sprite.h"

#include "util/debug.h"

static shader::Shader           s_sprite_shader;
static mat4                     u_projection = GLM_MAT2_IDENTITY_INIT;
static atlas::Atlas             s_tex_atlas;

static renderer::RState_Sprites batch;
static renderer::RSprite        block;
static renderer::RSprite        play_field;
static renderer::RSprite        play_field_outline;

#define BLOCK_SIZE  (SCREEN_HEIGHT/24.0f)

namespace game {
    void init(void)
    {
        glEnable(GL_DEPTH_TEST);

        renderer::init_sprites(batch);
        texture::set_onload_flipv(false);
        platform::set_vsync(true);

        shader::create(s_sprite_shader, "./assets/shader/sprite.vert", "./assets/shader/sprite.frag");
        atlas::create(s_tex_atlas, "./assets/sprites/metrix_tex.png", 64, 64);
        
        
        batch.shader_id = s_sprite_shader.id;
        
        
        block.atlas_dims = &s_tex_atlas;
        play_field.atlas_dims = &s_tex_atlas;
        play_field_outline.atlas_dims = &s_tex_atlas;
        
    }
    
    void update(double dTime)
    {
        (void) dTime;
        play_field.uv_index = 0;
        play_field.transform = { .position = {SCREEN_CENTER_X, SCREEN_CENTER_Y}, .scale = {BLOCK_SIZE*10, BLOCK_SIZE*20}};
        play_field_outline.uv_index = 1;
        play_field_outline.transform = { .position = {SCREEN_CENTER_X, SCREEN_CENTER_Y}, .scale = {BLOCK_SIZE*10 + 8, BLOCK_SIZE*20 + 8}};

        block.uv_index = 2;
        block.transform = { .position = {SCREEN_CENTER_X, SCREEN_CENTER_Y}, .scale = {BLOCK_SIZE, BLOCK_SIZE}};

        glm_mat4_identity(u_projection);
        glm_ortho(0.0f, (float)platform::g_window.width, (float)platform::g_window.height, 0, -1.0f, 1.0f, u_projection);
    }

    void render(void)
    {
        shader::bind(s_sprite_shader);
        shader::set_uniform_mat4(s_sprite_shader, "u_projection", u_projection);

        renderer::begin_sprites(batch);
        renderer::submit_sprites(batch, block);
        renderer::submit_sprites(batch, play_field);
        renderer::submit_sprites(batch, play_field_outline);
        renderer::end_sprites(batch);
    }
    void shutdown(void)
    {

    }
}