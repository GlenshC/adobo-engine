#include <glad/gl.h>
#include "core/game.h"
#include "resource/atlas.h"
#include "renderer/shader.h"
#include "renderer/r_sprite.h"

#include "util/logger.h"

static shader::Shader           s_sprite_shader;
static atlas::Atlas             s_tex_atlas;

static renderer::RSprite        block;
static renderer::RState_Sprites batch;
namespace game {
    void init(void)
    {
        renderer::init_sprites(batch);

        shader::create(s_sprite_shader, "./assets/shader/sprite.vert", "./assets/shader/sprite.frag");
        atlas::create(s_tex_atlas, "./assets/sprites/debug.png", 32, 32);
        
        batch.shader_id = s_sprite_shader.id;
        block.atlas_dims = &s_tex_atlas;
        block.uv_index = 2;
        block.transform = { .position = {255, 255}, .scale = {2, 2}};
        
        shader::bind(s_sprite_shader);
        shader::set_uniform_mat4(s_sprite_shader, "u_projection", GLM_MAT4_IDENTITY);
        
        renderer::begin_sprites(batch);
        renderer::submit_sprites(batch, block);
        renderer::flush_sprites(batch);
        
    }
    
    void update(double dTime)
    {
        (void) dTime;
    }
    
    void render(void)
    {
        renderer::draw_sprites(batch);
    }
    void shutdown(void)
    {
    }
}