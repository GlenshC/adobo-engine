#pragma once

#include "types.h"
#include "components/transform.h"
#include "resource/graphics/texture.h"

#define MAX_ENTITIES 200
namespace ecs 
{
    const i32 INVALID_ENTITY_ID = -1;
    typedef i32 Entity2D;

    struct Entity2Dref
    {
        core::Xform2Dref  transform;
        adobo::vec3<u16>  &frames;
        texture::Texture  &tex_id;
        adobo::vec2f      &velocity;

        Entity2Dref& operator=(const Entity2Dref& other)
        {
            transform = other.transform;
            frames    = other.frames;
            tex_id    = other.tex_id;
            velocity  = other.velocity;
            
            return  *this;
        }
    };
    
    template<i32 MAX_E>
    struct Entity2DSoa 
    {
        core::Xform2Dsoa<MAX_E> transform         = {};
        texture::Texture        texture_id[MAX_E] = {};  // our own id not gl (TODO: REV TEX)
        adobo::vec3<u16>        frames[MAX_E]     = {}; // curr, base, offset
        adobo::vec2f            velocity[MAX_E]   = {};
        
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
                .transform = transform[index],
                .frames    = frames[index],
                .tex_id    = texture_id[index],
                .velocity  = velocity[index],
            };
        }

        Entity2Dref operator()(Entity2D id)
        {
            i32 index = sparse[id];
            return (*this)[index];
        }

        i32& size(void)
        {
            return sparse.size;
        }
        
    };

    extern Entity2DSoa<MAX_ENTITIES> g_entities;
    
    Entity2Dref create_entity(Entity2D &id_out);
    Entity2D    create_entity(void);
    void        remove(Entity2D &id);
    void        update_all();


    /*
    struct entity3D
    {

    };
    */
}