#pragma once

#include "types.h"
#include "components/transform.h"
#include "res/graphics/texture.h"

#define MAX_ENTITIES 200
namespace ecs 
{
    /* CONSTANTS */
    const i32 INVALID_ENTITY_ID = -1;
    
    /* TYPES */
    typedef i32 Entity2DID;
    struct Entity2Dref;
    struct Entity2D;
    template<i32 MAX_E>
    struct Entity2DSoa;
    template <i32 N>
    struct Entity2DGroup;
    
    /* GLOBALS */
    extern Entity2DSoa<MAX_ENTITIES> g_entities;

    /* FUNCTIONS */
    Entity2Dref create(
        Entity2D &entity_out, 
        const texture::Texture &tex, 
        const adobo::vec4f &tex_uv, 
        const f32 pos_x, 
        const f32 pos_y, 
        const f32 scale_x, 
        const f32 scale_y
    );
    Entity2Dref create(
        Entity2D &entity_out, 
        const texture::Texture &tex, 
        const adobo::vec4f &tex_uv
    );

    template<i32 N>
    Entity2Dref create(
        Entity2DGroup<N> &ents, 
        const texture::Texture &tex, 
        const adobo::vec4f &tex_uv, 
        const f32 pos_x, 
        const f32 pos_y, 
        const f32 scale_x, 
        const f32 scale_y
    );
    Entity2Dref create(Entity2D &entity_out);
    Entity2Dref create_entity(Entity2D &entity_out);
    void        remove_entity(Entity2D &entity);
    void        update_all(f32 delta_time);

    adobo::vec4f get_aabb(Entity2D &ent);


    /* TYPE DEFS */
    struct Entity2Dref {
        core::Xform2Dref  transform;
        adobo::vec2f      &position;
        adobo::vec2f      &scale;
        adobo::vec3f      &rotation;
        adobo::vec4<i16>  &frames;
        texture::Texture  &tex;
        adobo::vec4f      &tex_uv;
        adobo::vec2f      &velocity;
        f32               &radius;

        Entity2Dref& operator=(const Entity2Dref& other);
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

        /* operators */
        Entity2Dref operator[](i32 index);
        Entity2Dref operator()(Entity2DID id);
        inline Entity2Dref operator()(Entity2D &entity);
        i32& size(void);
    };


    template <i32 N>
    struct Entity2DGroup
    {
        Entity2D data[N] = {};
        
        Entity2Dref operator()(void);
        void update();
    };

    /* METHODS AND OPERATORS */

    /* Entity2D */
    inline Entity2Dref Entity2D::operator()(void)
    {
        return g_entities(Entity2D::id);
    }


    /* Entity2Dref */
    inline Entity2Dref &Entity2Dref::operator=(const Entity2Dref &other)
    {
        transform = other.transform;
        position = other.position;
        scale = other.scale;
        rotation = other.rotation;
        frames = other.frames;
        tex = other.tex;
        tex_uv = other.tex_uv;
        velocity = other.velocity;
        radius = other.radius;

        return *this;
    }
    

    /* Entity2DSoa */
    template<i32 MAX_E>
    inline Entity2Dref Entity2DSoa<MAX_E>::operator[](i32 index)
    {
        return Entity2Dref{
            .transform = transform[index],
            .position = transform[index].position,
            .scale = transform[index].scale,
            .rotation = transform[index].rotation,
            .frames = frames[index],
            .tex = textures[index],
            .tex_uv = tex_uv[index],
            .velocity = velocity[index],
            .radius = radius[index],
        };
    }

    template<i32 MAX_E>
    inline Entity2Dref Entity2DSoa<MAX_E>::operator()(Entity2DID id)
    {
        return (*this)[sparse[id]];
    }

    template<i32 MAX_E>
    inline Entity2Dref Entity2DSoa<MAX_E>::operator()(Entity2D &entity)
    {
        return (*this)[sparse[entity.id]];
    }

    template<i32 MAX_E>
    inline i32 &Entity2DSoa<MAX_E>::size(void)
    {
        return sparse.size;
    }


    /* Entity2DGroup */
    template <i32 N>
    Entity2Dref Entity2DGroup<N>::operator()(void)
    {
        return data[0]();
    }

    template <i32 N>
    void Entity2DGroup<N>::update()
    {
        const auto &first = data[0]();
        f32 pos_x = first.position.x;
        f32 pos_y = first.position.y;
        f32 scale_x = first.scale.x;
        f32 scale_y = first.scale.y;
        for (int i = 1; i < N; i++)
        {
            data[i]().position = {pos_x + i * scale_x, pos_y};
            data[i]().scale = {scale_x, scale_y};
        }
    }




    /* FUNCTION DEFS */

    template <i32 N>
    inline Entity2Dref create(Entity2DGroup<N> &ents, const texture::Texture &tex, const adobo::vec4f &tex_uv, f32 pos_x, f32 pos_y, f32 scale_x, f32 scale_y)
    {
        for (i32 i = 0; i < N; i++)
        {
            ecs::Entity2Dref e = ecs::create(ents.data[i]);
            e.tex = tex;
            e.tex_uv = tex_uv;
            e.position = {pos_x + i * scale_x, pos_y};
            e.scale = {scale_x, scale_y};
        }
        return ents();
    }
}
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