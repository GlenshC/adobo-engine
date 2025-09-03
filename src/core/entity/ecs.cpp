#include "core/entity/ecs.h"
#include "core/entity/hitbox.h"
#include "core/clock.h"
#include <algorithm>
namespace ecs 
{
    Entity2DManager g_entities;
    HitboxManager   g_hitboxes;

    void init_Entity2DManager()
    {
        g_entities.init(ADOBO_ENGINE_MIN_ENTITIES2D);
        init_HitboxManager();
    }

    Entity2Dref create(Entity2D &entity_out)
    {
        return create_entity(entity_out);    
    }
    
    Entity2Dref create(
        Entity2D &entity_out, 
        const texture::Texture &tex, 
        const adobo::vec4f &tex_uv, 
        const f32 pos_x, 
        const f32 pos_y, 
        const f32 scale_x, 
        const f32 scale_y
    )
    {
        auto data = create_entity(entity_out);
        data.tex = tex;
        data.tex_uv = tex_uv;
        data.position.x = pos_x;
        data.position.y = pos_y;
        data.scale.x = scale_x;
        data.scale.y = scale_y;
        return entity_out();
    }

    Entity2Dref create(
        Entity2D &entity_out, 
        const texture::Texture &tex, 
        const adobo::vec4f &tex_uv
    )
    {
        auto data = create_entity(entity_out);
        data.tex = tex;
        data.tex_uv = tex_uv;
        return entity_out();
    }

    Entity2Dref create_entity(Entity2D &entity_out)
    {
        if (g_entities.size + ADOBO_ENGINE_REALLOC_MARGIN >= g_entities.capacity) 
        {
            g_entities.reserve(g_entities.capacity << 1);
        }
        i32 index;
        entity_out.id = g_entities.sparse.create_id(index);

        g_entities.hitbox[index].id = g_hitboxes.sparse.invalid_id();
        // g_entities.hitbox[index].aabb.data = nullptr;
        // g_entities.hitbox[index].type      = 0;
        g_entities.size++;
        return g_entities(entity_out.id);
    }

    void remove_entity(Entity2D &entity)
    {
        i32 index         = (i32)g_entities.sparse.remove(entity.id);
        i32 last_index    = (i32)g_entities.sparse.size;

        g_entities.hitbox[index].id = g_hitboxes.sparse.invalid_id();
        // if (g_entities.hitbox[index].aabb.data)
        //     std::free(g_entities.hitbox[index].aabb.data);
        // g_entities.hitbox[index].aabb.data = nullptr;
        // g_entities.hitbox[index].type      = 0;
        
        g_entities[index] = g_entities[last_index];
        entity.id = INVALID_ENTITY_ID;

        g_entities.size--;
    }

    adobo::vec4f get_aabb(Entity2D &ent)
    {
        ecs::Entity2Dref data = ent();

        f32 rx = data.scale.x * 0.5f;
        f32 ry = data.scale.y * 0.5f;
        return adobo::vec4f{
            data.position.x - rx, data.position.y - ry,
            data.position.x + rx, data.position.y + ry};
    }

    bool hb_is_hit(Entity2D ent, const adobo::vec2f &point)
    {
        return hb_is_point_inside(
            ent.get<Tag_Hitbox>().get<HitboxAABB>(), 
            ent.get<Tag_Position>(),
            ent.get<Tag_Scale>(),
            point
        );
    }
    bool hb_is_hit(Entity2D ent, const adobo::vec2f &point, f32 radius)
    {
        return hb_is_circle_overlapping(
            ent.get<Tag_Hitbox>().get<HitboxAABB>(), 
            ent.get<Tag_Position>(),
            ent.get<Tag_Scale>(),
            point,
            radius
        );
    }
    bool hb_is_hit(Entity2D ent, const adobo::vec3f &point)
    {
        return hb_is_circle_overlapping(
            ent.get<Tag_Hitbox>().get<HitboxAABB>(), 
            ent.get<Tag_Position>(),
            ent.get<Tag_Scale>(),
            (adobo::vec2f&) point,
            point.z
        );
    }

    bool hb_is_point_inside(
        const ecs::HitboxAABB &aabb,
        const adobo::vec3f &ent_pos,
        const adobo::vec2f &ent_scale,
        const adobo::vec2f &point
    )
    {
        // Compute entity top-left (world space)
        adobo::vec2f ent_topleft = {
            ent_pos.x - (0.5f * ent_scale.x),
            ent_pos.y - (0.5f * ent_scale.y)
        };

        for (int i = 0; i < aabb.size; i++)
        {
            // Build world-space rect
            float rx = ent_topleft.x + aabb.data[i].x * aabb.data[i].z * ent_scale.x;
            float ry = ent_topleft.y + aabb.data[i].y * aabb.data[i].w * ent_scale.y;
            float rw = aabb.data[i].z * ent_scale.x;
            float rh = aabb.data[i].w * ent_scale.y;

            // Bounds check
            if (point.x >= rx && point.x <= rx + rw &&
                point.y >= ry && point.y <= ry + rh)
            {
                return true; // inside this rect
            }
        }

        return false; // no hit
    }

    bool hb_is_circle_overlapping(
        const ecs::HitboxAABB &aabb,
        const adobo::vec3f &ent_pos,
        const adobo::vec2f &ent_scale,
        const adobo::vec2f &point,
        f32 radius
    )
    {
        adobo::vec2f ent_topleft = {
            ent_pos.x - (ent_scale.x),
            ent_pos.y - (ent_scale.y)
        };

        for (int i = 0; i < aabb.size; i++)
        {
            // World-space rect
            float rx = ent_topleft.x + aabb.data[i].x * ent_scale.x;
            float ry = ent_topleft.y + aabb.data[i].y * ent_scale.y;
            float rw = aabb.data[i].z * ent_scale.x;
            float rh = aabb.data[i].w * ent_scale.y;

            // Clamp circle center to rect bounds (find closest point on rect)
            float closestX = std::max(rx, std::min(point.x, rx + rw));
            float closestY = std::max(ry, std::min(point.y, ry + rh));

            // Distance from circle center to that point
            float dx = point.x - closestX;
            float dy = point.y - closestY;
            float dist2 = dx * dx + dy * dy;

            if (dist2 <= radius * radius)
                return true; // circle intersects this rect
        }

        return false;
    }

    // hitbox
    void HitboxAABB::init(const HitboxAABB &aabb, const adobo::vec3f &ent_pos, const adobo::vec2f &ent_scale)
    {
        this->init(aabb.size);

        for (int i =0; i < aabb.size; i++)
        { 
            this->data[i].x = ent_pos.x + aabb.data[i].x * ent_scale.x;
            this->data[i].y = ent_pos.y + aabb.data[i].y * ent_scale.y;
            this->data[i].z = aabb.data[i].z * ent_scale.x;
            this->data[i].w = aabb.data[i].w * ent_scale.y;
        }
        this->id   = aabb.id;
        this->type = aabb.type | ecs::HITBOX_FOR_ENTITY; 
        this->size = aabb.size;
    }

    void HitboxCircle::init(const HitboxCircle &circle, const adobo::vec3f &ent_pos, const adobo::vec2f &ent_scale)
    {
        (void) circle, (void) ent_pos, (void) ent_scale;
        // TODO
    }

    void HitboxAABB::update(const adobo::vec3f &ent_pos, const adobo::vec2f &ent_scale)
    {
        HitboxIndex index = g_hitboxes.get_val<Hitbox>(this->id);
        if (
            g_hitboxes.is_valid(this->id) &&
            this->data &&
            this->type & HITBOX_TYPE_AABB_ENT && 
            g_hitboxes.get_val<HitboxType>(index) & HITBOX_TYPE_AABB
        )
        {
            HitboxAABB &aabb = g_hitboxes.get<HitboxUnion>(index).aabb;
            
            if (aabb.size > capacity && this->reserve(aabb.size))
            {
                DEBUG_ERR("HitboxAABB::update(): Error reserving memory.\n");
                return;
            }

            for  (int i = 0; i < aabb.size; i++)
            {
                this->data[i].x = ent_pos.x + aabb.data[i].x * ent_scale.x;
                this->data[i].y = ent_pos.y + aabb.data[i].y * ent_scale.y;
                this->data[i].z = aabb.data[i].z * ent_scale.x;
                this->data[i].w = aabb.data[i].w * ent_scale.y;
            }
        }
    }
}