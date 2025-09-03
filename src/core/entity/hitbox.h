#pragma once

#include "ggb/sparse.h"
#include "types.h"
#include <cstdlib>
#include "util/debug.h"
#include "core/constants.h"


namespace ecs
{
    /* TAGS */
    typedef u16 HitboxID, HitboxIndex;
    struct HitboxAABB;
    struct HitboxCircle;
    struct Hitbox;
    struct HitboxManager;
    union HitboxUnion;

    extern HitboxManager g_hitboxes;

    inline void init_HitboxManager();

    struct HitboxAABB 
    {
        u16 type, size, capacity, id;
        adobo::vec4f *data;

        void init(u16 n);
        i32  reserve(u16 new_cap);

        // soon
        void init(const HitboxAABB &aabb, const adobo::vec3f& ent_pos, const adobo::vec2f &ent_scale);
        void update(const adobo::vec3f &ent_pos, const adobo::vec2f &ent_scale);
        
        adobo::vec4f &emplace_back(f32 left, f32 top, f32 right, f32 bottom);
        adobo::vec4f &emplace_back(adobo::vec4f &aabb);
        adobo::vec4f &operator[](u16 i);
    };

    struct HitboxCircle
    {
        u16 type, size, capacity, id;
        adobo::vec3f *data;

        void init(u16 n);
        void init(const HitboxCircle &circle, const adobo::vec3f& ent_pos, const adobo::vec2f &ent_scale);

        i32  reserve(u16 new_cap);
        adobo::vec3f &emplace_back(f32 x, f32 y, f32 radius);
        adobo::vec3f &emplace_back(adobo::vec3f &circle);
        adobo::vec3f &operator[](u16 i);
    };
    
    union HitboxUnion
    {
        u16 type;
        HitboxAABB   aabb;
        HitboxCircle circle;
    };
    
    enum HitboxType
    {
        HITBOX_TYPE_INVALID     = 0,
        HITBOX_TYPE_AABB        = 0b001,
        HITBOX_TYPE_CIRCLE      = 0b010,
        HITBOX_FOR_ENTITY       = 0b100,
        HITBOX_TYPE_AABB_ENT    = 0b101,
        HITBOX_TYPE_CIRCLE_ENT  = 0b110,
    };
    
    struct Hitbox
    {
        HitboxID id;

        bool is_valid();

        template<typename Tag>
        auto& get(HitboxIndex);

        template<typename Tag>
        auto& get();

        template<typename Tag>
        auto get_val();

        template<typename Tag>
        auto get_val(HitboxIndex);
    };


    struct HitboxManager
    {
        char                 *_bp;
        ecs::HitboxUnion     *hitboxes;
        ggb::Sparse<HitboxID> sparse;

        size_t size;
        size_t capacity;

        void init(size_t n);
        i32  reserve(size_t new_cap);
        bool is_valid(HitboxID id);
        HitboxUnion &operator[](size_t index);

        template<typename T>
        auto& get(u16 index);

        template<typename T>
        auto get_val(u16 index);
    };


    /*********************
     *                   *
     * FUNCTIONS         *
     *                   *
     *********************/

    template<HitboxType T>
    inline auto& create_hitbox(Hitbox&);

    template<>
    inline auto& create_hitbox<HITBOX_TYPE_AABB>(Hitbox &hitbox_out)
    {
        if (g_hitboxes.size + ADOBO_ENGINE_REALLOC_MARGIN >= g_hitboxes.capacity) 
        {
            g_hitboxes.reserve(g_hitboxes.capacity << 1);
        }
        HitboxID index = 0;
        hitbox_out.id = g_hitboxes.sparse.create_id(index);

        g_hitboxes.size++;
        g_hitboxes.hitboxes[index].aabb.init(1);
        g_hitboxes.hitboxes[index].type = HITBOX_TYPE_AABB;
        g_hitboxes.hitboxes[index].aabb.id = hitbox_out.id;
        return g_hitboxes[index].aabb;
    }

    template<>
    inline auto& create_hitbox<HITBOX_TYPE_CIRCLE>(Hitbox &hitbox_out)
    {
        if (g_hitboxes.size + ADOBO_ENGINE_REALLOC_MARGIN >= g_hitboxes.capacity) 
        {
            g_hitboxes.reserve(g_hitboxes.capacity << 1);
        }
        HitboxID index = 0;
        hitbox_out.id = g_hitboxes.sparse.create_id(index);

        g_hitboxes.size++;
        g_hitboxes.hitboxes[index].circle.init(1);
        g_hitboxes.hitboxes[index].type = HITBOX_TYPE_CIRCLE;
        g_hitboxes.hitboxes[index].circle.id = hitbox_out.id;
        return g_hitboxes[index].circle;
    }

    /*********************
     *                   *
     * METHODS           *
     *                   *
     *********************/

    /* HitboxAABB */
    inline adobo::vec4f& HitboxAABB::emplace_back(f32 x, f32 y, f32 width, f32 height)
    {
        if (HitboxAABB::size >= capacity)
        {
            HitboxAABB::reserve(capacity << 1);
        }
        HitboxAABB::data[size].x = x;         
        HitboxAABB::data[size].y = y;         
        HitboxAABB::data[size].z = width;         
        HitboxAABB::data[size].w = height; 

        return HitboxAABB::data[size++];  
    }

    inline adobo::vec4f& HitboxAABB::emplace_back(adobo::vec4f &item)
    {
        if (HitboxAABB::size >= capacity)
        {
            HitboxAABB::reserve(capacity << 1);
        }
        return HitboxAABB::data[size++] = item;         
    }

    inline void HitboxAABB::init(u16 n)
    {
        if (n > 0)
        {
            char *mem = nullptr;
            mem = (char *)std::malloc(
                sizeof(*data));
            if (!mem)
            {
                DEBUG_ERR("HitboxAABB: BAD ALLOCATION\n");
                return;
            }

            data = (adobo::vec4f *)mem;

            size = 0;
            capacity = n;
            DEBUG_LOG("Initialized HitboxAABB(%u).\n", capacity);
            return;
        }
        DEBUG_LOG("Failed to initialized HitboxAABB.\n");
    }

    inline i32 HitboxAABB::reserve(u16 new_cap)
    {
        if (new_cap <= capacity)
            return 1;

        char *mem = (char *)std::realloc(
            data,
            sizeof(*data) * new_cap);
        if (!mem)
        {
            DEBUG_ERR("HitboxAABB: Resize Error\n");
            return 1;
        }
        data = (adobo::vec4f *)mem;
        capacity = new_cap;
        DEBUG_LOG("Reserved HitboxAABB(%u).\n", capacity);
        return 0;
    }

    /* Hitbox Circle */

    inline adobo::vec3f &HitboxCircle::operator[](u16 i)
    {
        return HitboxCircle::data[i]; 
    }
    
    inline adobo::vec3f &HitboxCircle::emplace_back(f32 x, f32 y, f32 radius)
    {
        if (HitboxCircle::size >= capacity)
        {
            HitboxCircle::reserve(capacity << 1);
        }
        HitboxCircle::data[size].x = x;         
        HitboxCircle::data[size].y = y;         
        HitboxCircle::data[size].z = radius;         

        return HitboxCircle::data[size++];  
    }

    inline adobo::vec3f &HitboxCircle::emplace_back(adobo::vec3f &item)
    {
        if (HitboxCircle::size >= capacity)
        {
            HitboxCircle::reserve(capacity << 1);
        }
        return HitboxCircle::data[size++] = item; 
    }

    inline void HitboxCircle::init(u16 n)
    {
        if (n > 0)
        {
            char *mem = nullptr;
            mem = (char *)std::malloc(
                sizeof(*data));
            if (!mem)
            {
                DEBUG_ERR("HitboxCircle: BAD ALLOCATION\n");
                return;
            }

            data = (adobo::vec3f *)mem;

            size = 0;
            capacity = n;
            DEBUG_LOG("Initialized HitboxCircle(%u).\n", capacity);
            return;
        }
        DEBUG_LOG("Failed to initialized HitboxCircle.\n");
    }

    inline i32 HitboxCircle::reserve(u16 new_cap)
    {
        if (new_cap <= capacity)
            return 1;

        char *mem = (char *)std::realloc(
            data,
            sizeof(*data) * new_cap);
        if (!mem)
        {
            DEBUG_ERR("HitboxCircle: Resize Error\n");
            return 1;
        }
        data = (adobo::vec3f *)mem;
        capacity = new_cap;
        DEBUG_LOG("Reserved HitboxCircle(%u).\n", capacity);
        return 0;
    }

    /* HitboxManager */
    inline bool HitboxManager::is_valid(HitboxID id)
    {
        return HitboxManager::sparse.is_valid(id);
    }

    inline void HitboxManager::init(size_t n)
    {
        if (n > 0)
        {
            char *mem = nullptr;
            mem = (char *)std::calloc(
                1,
                sizeof(*hitboxes) * n
            );
            if (!mem)
            {
                DEBUG_ERR("HitboxManager: BAD ALLOCATION\n");
                return;
            }
            
            _bp = mem;
            hitboxes = (HitboxUnion *) (mem);
            sparse.init(n);
            size = 0;
            capacity = n;
            DEBUG_LOG("Initialized HitboxManager(%zu).\n", capacity);
            return;
        }
        DEBUG_LOG("Failed to initialized HitboxManager.\n");
    }

    inline i32 HitboxManager::reserve(size_t new_cap)
    {
        if (new_cap <= capacity)
            return 1;

        char *mem = (char *)std::realloc(
            _bp,
            sizeof(*hitboxes) * new_cap
        );
        if (!mem)
        {
            DEBUG_ERR("HitboxManager: Resize Error\n");
            return 1;
        }

        _bp = mem;
        hitboxes = (HitboxUnion *) (mem); 
        capacity = new_cap;
        sparse.reserve(new_cap);
        DEBUG_LOG("Reserved HitboxManager(%zu).\n", capacity);
        return 0; 
    }

    inline HitboxUnion&
    HitboxManager::operator[](size_t index)
    {
        return hitboxes[index];
    }

    inline void init_HitboxManager()
    {
        g_hitboxes.init(ADOBO_ENGINE_MIN_HITBOXES);
    }
    
    
    template<>
    inline auto HitboxManager::get_val<Hitbox>(HitboxID id)
    {
        return sparse[id];
    }

    template<>
    inline auto HitboxManager::get_val<HitboxType>(u16 index)
    {
        return hitboxes[index].type;
    }

    template<>
    inline auto& HitboxManager::get<HitboxUnion>(u16 index)
    {
        return hitboxes[index];
    }


    /* Hitbox */


    inline bool Hitbox::is_valid()
    {
        return g_hitboxes.is_valid(id);
    }

    template<>
    inline auto Hitbox::get_val<Hitbox>()
    {
        return g_hitboxes.get_val<Hitbox>(id);
    }
    
    template<>
    inline auto Hitbox::get_val<HitboxType>(HitboxIndex index)
    {
        return g_hitboxes.get_val<HitboxType>(index);
    }

    template<>
    inline auto& Hitbox::get<HitboxUnion>(HitboxIndex index)
    {
        return g_hitboxes.get<HitboxUnion>(index);
    }

    template<>
    inline auto& Hitbox::get<HitboxAABB>(HitboxIndex index)
    {
        return g_hitboxes.get<HitboxUnion>(index).aabb;
    }

    template<>
    inline auto& Hitbox::get<HitboxCircle>(HitboxIndex index)
    {
        return g_hitboxes.get<HitboxUnion>(index).circle;
    }


    /*  */
    template<>
    inline auto Hitbox::get_val<HitboxType>()
    {
        return g_hitboxes.get_val<HitboxType>(g_hitboxes.get_val<Hitbox>(id));
    }

    template<>
    inline auto& Hitbox::get<HitboxUnion>()
    {
        return g_hitboxes.get<HitboxUnion>(g_hitboxes.get_val<Hitbox>(id));
    }

    template<>
    inline auto& Hitbox::get<HitboxAABB>()
    {
        return g_hitboxes.get<HitboxUnion>(g_hitboxes.get_val<Hitbox>(id)).aabb;
    }

    template<>
    inline auto& Hitbox::get<HitboxCircle>()
    {
        return g_hitboxes.get<HitboxUnion>(g_hitboxes.get_val<Hitbox>(id)).circle;
    }

}