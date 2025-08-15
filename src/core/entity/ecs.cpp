#include "core/entity/ecs.h"
#include "core/clock.h"
namespace ecs 
{
    Entity2DSoa<MAX_ENTITIES> g_entities;

    Entity2D create_entity(void)
    {
        return {g_entities.sparse.create_id()};
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
    void update_all(f32 delta_time)
    {
        for (i32 i = 0; i < g_entities.size(); i++)
        {
            g_entities[i].position.x += g_entities[i].velocity.x * delta_time;
            g_entities[i].position.y += g_entities[i].velocity.y * delta_time;

            if (clk::g_time.frame_timer[g_entities[i].frames[3]] == 0)
            {
                g_entities[i].frames[0]++;
                if ( g_entities[i].frames[0] > (g_entities[i].frames[1] + g_entities[i].frames[2]))
                {
                    g_entities[i].frames[0] = g_entities[i].frames[1];
                }
            }
        }
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