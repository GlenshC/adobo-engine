#pragma once

#include "ggb/sparse.h"
#include "types.h"
#include <cstdlib>
#include "util/debug.h"
#include "core/constants.h"


namespace ecs
{
    typedef i32 HitboxID;
    struct HitboxAABB;
    struct HitboxCircle;
    struct Hitbox;
    struct HitboxManager;
    union HitboxUnion;

    extern HitboxManager g_hitboxes;

    inline void init_HitboxManager();

    struct HitboxAABB 
    {
        adobo::vec4f *aabb;
        u32 size, capacity;

        void init(u32 n);
        i32  reserve(u32 new_cap);
        // adobo::vec4f &emplace_back()
    };

    struct HitboxCircle
    {
        adobo::vec3f *circle;
        u32 size, capacity;

        void init(u32 n);
        i32  reserve(u32 new_cap);
    };

    union HitboxUnion
    {
        HitboxAABB   aabb;
        HitboxCircle circle;
    };

    enum HitboxType
    {
        HITBOX_TYPE_AABB,
        HITBOX_TYPE_CIRCLE 
    };

    struct Hitbox
    {
        HitboxID id;

        HitboxAABB   &get_aabb();
        HitboxCircle &get_circle();
        HitboxType   &get_type();
    };

    struct HitboxManager
    {
        char                 *_bp;
        ecs::HitboxUnion     *hitboxes;
        ecs::HitboxType      *type;
        ggb::Sparse<HitboxID>      sparse;

        size_t size;
        size_t capacity;

        void init(size_t n);
        i32  reserve(size_t new_cap);
        HitboxType &get_type(HitboxID hid);
        HitboxType &get_type(Hitbox &hitbox);

        HitboxUnion &operator[](size_t index);
        HitboxUnion &operator()(HitboxID hid);
        HitboxUnion &operator()(Hitbox &hitbox);
    };

    /* HitboxAABB */

    inline void HitboxAABB::init(u32 n)
    {
        if (n > 0)
        {
            char *mem = nullptr;
            mem = (char *)std::malloc(
                sizeof(*aabb));
            if (!mem)
            {
                DEBUG_ERR("HitboxAABB: BAD ALLOCATION\n");
                return;
            }

            aabb = (adobo::vec4f *)mem;

            size = 0;
            capacity = n;
            DEBUG_LOG("Initialized HitboxAABB(%u).\n", capacity);
            return;
        }
        DEBUG_LOG("Failed to initialized HitboxAABB.\n");
    }

    inline i32 HitboxAABB::reserve(u32 new_cap)
    {
        if (new_cap <= capacity)
            return 1;

        char *mem = (char *)std::realloc(
            aabb,
            sizeof(*aabb) * new_cap);
        if (!mem)
        {
            DEBUG_ERR("HitboxAABB: Resize Error\n");
            return 1;
        }
        aabb = (adobo::vec4f *)mem;
        capacity = new_cap;
        DEBUG_LOG("Reserved HitboxAABB(%u).\n", capacity);
        return 0;
    }

    /* Hitbox Circle */

    inline void HitboxCircle::init(u32 n)
    {
        if (n > 0)
        {
            char *mem = nullptr;
            mem = (char *)std::malloc(
                sizeof(*circle));
            if (!mem)
            {
                DEBUG_ERR("HitboxCircle: BAD ALLOCATION\n");
                return;
            }

            circle = (adobo::vec3f *)mem;

            size = 0;
            capacity = n;
            DEBUG_LOG("Initialized HitboxCircle(%u).\n", capacity);
            return;
        }
        DEBUG_LOG("Failed to initialized HitboxCircle.\n");
    }

    inline i32 HitboxCircle::reserve(u32 new_cap)
    {
        if (new_cap <= capacity)
            return 1;

        char *mem = (char *)std::realloc(
            circle,
            sizeof(*circle) * new_cap);
        if (!mem)
        {
            DEBUG_ERR("HitboxCircle: Resize Error\n");
            return 1;
        }
        circle = (adobo::vec3f *)mem;
        capacity = new_cap;
        DEBUG_LOG("Reserved HitboxCircle(%u).\n", capacity);
        return 0;
    }

    /* Hitbox */
    inline HitboxAABB &Hitbox::get_aabb()
    {
        return g_hitboxes(id).aabb;
    }

    inline HitboxCircle &Hitbox::get_circle()
    {
        return g_hitboxes(id).circle;
    }
    
    inline HitboxType &Hitbox::get_type()
    {
        return g_hitboxes.get_type(id);
    }

    /* HitboxManager */
    inline void HitboxManager::init(size_t n)
    {
        if (n > 0)
        {
            char *mem = nullptr;
            mem = (char *)std::malloc(
                sizeof(*hitboxes) * n +
                sizeof(*type) * n
            );
            if (!mem)
            {
                DEBUG_ERR("HitboxManager: BAD ALLOCATION\n");
                return;
            }
            size_t offset = 0;
            
            _bp = mem;
            hitboxes = (HitboxUnion *)        (mem);
            type     = (HitboxType  *)        (mem + (offset += sizeof(*hitboxes)* n));

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
            sizeof(*hitboxes)     * new_cap +
            sizeof(*type) * new_cap
        );
        if (!mem)
        {
            DEBUG_ERR("HitboxManager: Resize Error\n");
            return 1;
        }
        size_t boffset = sizeof(*hitboxes) * capacity;

        size_t offset = 0;
        _bp = mem;
        hitboxes = (HitboxUnion *) (mem); 
        type     = (HitboxType  *) (mem + (offset += sizeof(*hitboxes) * new_cap)); 

        if (new_cap >= (capacity << 1)) // memcpy
        {
            memcpy(type,  (_bp + (boffset)), sizeof(*type) * capacity);
        }
        else
        {
            memmove(type, (_bp + (boffset)), sizeof(*type) * capacity);
        }
        capacity = new_cap;
        sparse.reserve(new_cap);
        DEBUG_LOG("Reserved HitboxManager(%zu).\n", capacity);
        return 0; 
    }

    inline HitboxType &HitboxManager::get_type(HitboxID hid)
    {
        return type[sparse[hid]];
    }

    inline HitboxType &HitboxManager::get_type(Hitbox &hitbox)
    {
        return type[sparse[hitbox.id]];
    }

    inline HitboxUnion&
    HitboxManager::operator[](size_t index)
    {
        return hitboxes[index];
    }

    inline HitboxUnion&
    HitboxManager::operator()(HitboxID hid)
    {
        return hitboxes[sparse[hid]];
    }

    inline HitboxUnion& 
    HitboxManager::operator()(Hitbox &hitbox)
    {
        return hitboxes[sparse[hitbox.id]];
    }

    inline void init_HitboxManager()
    {
        g_hitboxes.init(ADOBO_ENGINE_MIN_HITBOXES);
    }
    
}