#include "core/entity/ecs.h"
#include "core/clock.h"
namespace ecs 
{
    Entity2DManager g_entities;

    void init()
    {
        g_entities.init(MIN_ENTITIES);
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
        entity_out.id = g_entities.sparse.create_id();
        return g_entities(entity_out.id);
    }

    void remove_entity(Entity2D &entity)
    {
        i32 index      = g_entities.sparse.remove(entity.id);
        i32 last_index = g_entities.sparse.size;
        g_entities[index] = g_entities[last_index];

        entity.id = INVALID_ENTITY_ID;
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
}