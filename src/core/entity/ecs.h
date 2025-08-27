#pragma once

#include "types.h"
#include "renderer/texture.h"
#include "core/constants.h"


namespace ecs 
{
    /* CONSTANTS */
    const i32 INVALID_ENTITY_ID = -1;
    
    /* TYPES */
    typedef i32 Entity2DID;
    struct Entity2Dref;
    struct Entity2D;
    struct Entity2DManager;
    struct Multaabb;
    template <i32 N>
    struct Entity2DGroup;


    /* GLOBALS */
    extern Entity2DManager g_entities;

    /* FUNCTIONS */
    void         init_Entity2DManager();

    Entity2Dref  create(Entity2D &entity_out);
    Entity2Dref  create_entity(Entity2D &entity_out);
    void         remove_entity(Entity2D &entity);
    adobo::vec4f get_aabb(Entity2D &ent);

    template<i32 N>
    Entity2Dref create(Entity2DGroup<N> &ents, const texture::Texture &tex, const adobo::vec4f &tex_uv, const f32 pos_x, const f32 pos_y, const f32 scale_x, const f32 scale_y);
    Entity2Dref create(Entity2D &entity_out, const texture::Texture &tex, const adobo::vec4f &tex_uv, const f32 pos_x, const f32 pos_y, const f32 scale_x, const f32 scale_y);
    Entity2Dref create(Entity2D &entity_out, const texture::Texture &tex, const adobo::vec4f &tex_uv);

    /* TYPE DEFS */
    struct Entity2Dref {
        Multaabb         *&aabb;
        adobo::vec3f      &position;
        adobo::vec2f      &scale;
        adobo::vec3f      &rotation;
        texture::Texture  &tex;
        adobo::vec4f      &tex_uv;
        i32               &type;

        Entity2Dref& operator=(const Entity2Dref& other);
    };
    
    struct Entity2D
    {
        i32 id;

        inline Entity2Dref operator()(void);
    };

    struct Multaabb 
    {
        adobo::vec4f *aabb;
        size_t size;
    };

    struct Entity2DManager 
    {
        char             *_bp;
        Multaabb        **aabb;
        adobo::vec3f     *position;
        adobo::vec2f     *scale;
        adobo::vec3f     *rotation;
        texture::Texture *textures;
        adobo::vec4f     *tex_uv;
        i32              *type;
        ggb::Sparse<i32>  sparse;
        
        size_t size;
        size_t capacity;

        /* methods */
        inline void init(size_t n);
        inline int  reserve(size_t new_cap);
        
        /* operators */
        inline Entity2Dref operator[](size_t index);
        inline Entity2Dref operator()(Entity2DID id);
        inline Entity2Dref operator()(Entity2D &entity);
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
        aabb     = other.aabb;
        position = other.position;
        scale    = other.scale;
        rotation = other.rotation;
        tex      = other.tex;
        tex_uv   = other.tex_uv;
        type     = other.type;

        return *this;
    }
    

    /* Entity2DManager */
    inline void Entity2DManager::init(size_t n)
    {
        if (n > 0)
        {
            char *mem = nullptr;
            mem = (char *)std::malloc(
                sizeof(*aabb)     * n +
                sizeof(*position) * n +
                sizeof(*scale)    * n + 
                sizeof(*rotation) * n + 
                sizeof(*textures) * n + 
                sizeof(*tex_uv)   * n +
                sizeof(*type)     * n
            );
            if (!mem)
            {
                DEBUG_ERR("Sparse: BAD ALLOCATION\n");
                return;
            }

            size_t offset = 0;
            _bp      = mem;
            aabb     = (Multaabb **)        (mem);
            position = (adobo::vec3f *)     (mem + (offset += n * sizeof(*aabb)));
            scale    = (adobo::vec2f *)     (mem + (offset += n * sizeof(*position)));
            rotation = (adobo::vec3f *)     (mem + (offset += n * sizeof(*scale)));
            textures = (texture::Texture *) (mem + (offset += n * sizeof(*rotation)));
            tex_uv   = (adobo::vec4f *)     (mem + (offset += n * sizeof(*textures)));
            type     = (i32 *)              (mem + (offset += n * sizeof(*tex_uv)));
            
            sparse.init(n);
            size = 0;
            capacity = n;
            
            DEBUG_LOG("Initialized Entity2DManager(%zu).\n", capacity);
            return;
        }
        DEBUG_LOG("Failed to initialized Entity2DManager.\n");
    }

    inline int Entity2DManager::reserve(size_t new_cap)
    {
        if (new_cap <= capacity)
            return 1;

        char *mem = (char *)std::realloc(
            _bp,
            sizeof(*aabb)     * new_cap +
            sizeof(*position) * new_cap +
            sizeof(*scale)    * new_cap +
            sizeof(*rotation) * new_cap +
            sizeof(*textures) * new_cap +
            sizeof(*tex_uv)   * new_cap +
            sizeof(*type)     * new_cap
        );
        if (!mem)
        {
            DEBUG_ERR("Entity2DManager: Resize Error\n");
            return 1;
        }
        size_t boffset = 
            sizeof(*aabb)     * capacity +
            sizeof(*position) * capacity +
            sizeof(*scale)    * capacity +
            sizeof(*rotation) * capacity +
            sizeof(*textures) * capacity +
            sizeof(*tex_uv)   * capacity;

        size_t offset = 0;
        _bp = mem;
        aabb        = (Multaabb **)        (mem); 
        position    = (adobo::vec3f *)     (mem + (offset += sizeof(*aabb)     * new_cap)); 
        scale       = (adobo::vec2f *)     (mem + (offset += sizeof(*position) * new_cap));
        rotation    = (adobo::vec3f *)     (mem + (offset += sizeof(*scale)    * new_cap));
        textures    = (texture::Texture *) (mem + (offset += sizeof(*rotation) * new_cap));
        tex_uv      = (adobo::vec4f *)     (mem + (offset += sizeof(*textures) * new_cap));
        type        = (i32 *)              (mem + (offset += sizeof(*tex_uv)   * new_cap));

        if (new_cap >= (capacity << 1)) // memcpy
        {
            memcpy(type    , (_bp + (boffset)), sizeof(*type)     * capacity);
            memcpy(tex_uv  , (_bp + (boffset -= sizeof(*tex_uv)   * capacity)), sizeof(*tex_uv)   * capacity);
            memcpy(textures, (_bp + (boffset -= sizeof(*textures) * capacity)), sizeof(*textures) * capacity);
            memcpy(rotation, (_bp + (boffset -= sizeof(*rotation) * capacity)), sizeof(*rotation) * capacity);
            memcpy(scale   , (_bp + (boffset -= sizeof(*scale)    * capacity)), sizeof(*scale)    * capacity);
            memcpy(position, (_bp + (boffset -= sizeof(*position) * capacity)), sizeof(*position) * capacity);
        }
        else
        {
            memmove(type    , (_bp + (boffset)), sizeof(*type)     * capacity);
            memmove(tex_uv  , (_bp + (boffset -= sizeof(*tex_uv)   * capacity)), sizeof(*tex_uv)   * capacity);
            memmove(textures, (_bp + (boffset -= sizeof(*textures) * capacity)), sizeof(*textures) * capacity);
            memmove(rotation, (_bp + (boffset -= sizeof(*rotation) * capacity)), sizeof(*rotation) * capacity);
            memmove(scale   , (_bp + (boffset -= sizeof(*scale)    * capacity)), sizeof(*scale)    * capacity);
            memmove(position, (_bp + (boffset -= sizeof(*position) * capacity)), sizeof(*position) * capacity);
        }
        capacity = new_cap;
        sparse.reserve(new_cap);
        DEBUG_LOG("Reserved Entity2DManager(%zu).\n", capacity);
        return 0; 
    }

    inline Entity2Dref Entity2DManager::operator[](size_t index)
    {
        return Entity2Dref{
            aabb[index],
            position[index],
            scale[index],
            rotation[index],
            textures[index],
            tex_uv[index],
            type[index]
        };
    }

    inline Entity2Dref Entity2DManager::operator()(Entity2DID id)
    {
        return (*this)[sparse[id]];
    }

    inline Entity2Dref Entity2DManager::operator()(Entity2D &entity)
    {
        return (*this)[sparse[entity.id]];
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

    void update();
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