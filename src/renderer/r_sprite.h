#pragma once
#include <ggb/cglm.h>
#include "renderer/renderer.h"
#include "renderer/r_2D.h"
#include "renderer/shader.h"
#include "resource/atlas.h"
#define MAX_SPRITES 200

// renderer sprite 2D 3d
namespace renderer 
{
    struct RSprite 
    {
        atlas::Atlas *atlas_dims = nullptr;
        Xf2D transform;
        u32 uv_index = 0;
    };
    
    struct RData_S
    {
        mat4 model = GLM_MAT4_IDENTITY_INIT;
        u16  uv_index = 0;
        u16  tex_id = 0;
        u32  pad[3] = {}; // replace when adding
    };

    struct Spritesheet;
    
    
    struct RState_Sprites
    {
        atlas::Atlas   *atlas[MAX_TEX_SLOTS];
        u32             tex_slot[MAX_TEX_SLOTS];
        u32             vao, ssbo; //, ibo;
        u32             shader_id;
        i32             sprite_count;
        i32             tex_count;
        RData_S                     data[MAX_SPRITES];
        Xf2Dsoa<MAX_SPRITES>        sprites_arr; 
    };

// TODO: documentation
    void init_sprites(RState_Sprites& batch);
    void begin_sprites(RState_Sprites& batch);
    void submit_sprites(RState_Sprites &batch, RSprite &data);
    void end_sprites(RState_Sprites& batch);
    void flush_sprites(RState_Sprites& batch);
    void draw_sprites(RState_Sprites &batch);
}