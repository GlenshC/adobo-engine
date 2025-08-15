#pragma once

#include "types.h"
#include "components/transform.h"
#include "res/graphics/texture.h"

#define MAX_ENTITIES 200
namespace ecs 
{
    const i32 INVALID_ENTITY_ID = -1;
    typedef i32 Entity2DID;

    struct Entity2Dref
    {
        adobo::vec2f      &position;
        adobo::vec2f      &scale;
        f32               &rotation;
        adobo::vec4<i16>  &frames;
        texture::Texture  &tex;
        adobo::vec2f      &velocity;
        f32               &radius;

        Entity2Dref& operator=(const Entity2Dref& other)
        {
            position  = other.position;
            scale     = other.scale;
            rotation  = other.rotation;
            frames    = other.frames;
            tex       = other.tex;
            velocity  = other.velocity;
            radius    = other.radius;
            
            return  *this;
        }
    };
    
    struct Entity2D
    {
        i32 id;

        inline Entity2Dref operator()(void);
    };
    
    
    template<i32 MAX_E>
    struct Entity2DSoa 
    {
        core::Xform2Dsoa<MAX_E> transform         = {};
        texture::Texture        textures[MAX_E]   = {}; // our own id not gl
        adobo::vec4f            tex_uv[MAX_E]     = {}; // x, y, width, height (calculated before being sent to gpu)
        adobo::vec4<i16>        frames[MAX_E]     = {}; // curr, base, offset, frame_rate
        adobo::vec2f            velocity[MAX_E]   = {};
        f32                     radius[MAX_E]     = {};
        
        ggb::Sparse<MAX_E> sparse; // entity ids
        const i32   capacity = MAX_E;

        /*
        Transform	    position, rotation, scale	                        World transform
        Position	    x, y, z	                                            2D or 3D
        Rotation	    angle (2D) or pitch/yaw/roll or quaternion (3D)	    Orientation
        Scale   	    x, y, z	                                            Non-uniform scaling
        Velocity    	vx, vy, vz	                                        Movement
        Acceleration	ax, ay, az	                                        Optional physics
        Sprite          texture_id, color, size	                            2D rendering
        Model           mesh_id, material_id	                            3D rendering
        Animation       current_anim, frame, timer	                        Skeletal or sprite
        Input           move_dir, is_firing, etc.	                        Player control
        Health	        hp, max_hp	                                        Basic game logic
        Collider	    aabb, radius, etc.	                                Physics collision
        RigidBody	    mass, force, impulse, velocity	                    Real physics
        Camera	        zoom, view_matrix	                                Main/secondary cam
        Tag	            char* name, int layer	                            Optional identifiers
        Script	        callback_fn* on_update, etc.	                    Optional scripting
        */
       
        /* operators */
        Entity2Dref operator[](i32 index)
        {
            return Entity2Dref{
                .position  = transform[index].position,
                .scale     = transform[index].scale,
                .rotation  = transform[index].rotation,
                .frames    = frames[index],
                .tex       = textures[index],
                .velocity  = velocity[index],
                .radius    = radius[index],
            };
        }

        Entity2Dref operator()(Entity2DID id)
        {
            return (*this) [ sparse[id] ];
        }

        inline Entity2Dref operator()(Entity2D &entity)
        {
            return (*this) [ sparse[entity.id] ];
        }

        i32& size(void)
        {
            return sparse.size;
        }
        
    };

    extern Entity2DSoa<MAX_ENTITIES> g_entities;

    inline Entity2Dref Entity2D::operator()(void)
    {
        return g_entities(Entity2D::id);
    }
    
    
    Entity2Dref create_entity(Entity2D &entity_out);
    Entity2D    create_entity(void);
    void        remove_entity(Entity2D &entity);
    void        update_all(f32 delta_time);

    adobo::vec4f get_aabb(Entity2D &ent);
    /*
    struct entity3D
    {

    };
    */
}