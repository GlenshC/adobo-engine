#include "core/entity/ecs.h"
#include "core/clock.h"
namespace ecs 
{
    Entity2DSoa<MAX_ENTITIES> g_entities;

    Entity2D create_entity(void)
    {
        return g_entities.sparse.create_id();
    }

    Entity2Dref create_entity(Entity2D &id_out)
    {
        id_out = g_entities.sparse.create_id();
        return g_entities(id_out);
    }

    void remove(Entity2D &id)
    {
        i32 index      = g_entities.sparse.remove(id);
        i32 last_index = g_entities.sparse.size;
        g_entities[index] = g_entities[last_index];

        id = INVALID_ENTITY_ID;
    }
    void update_all()
    {
        for (i32 i = 0; i < g_entities.size(); i++)
        {
            g_entities[i].transform.position.x += g_entities[i].velocity.x;
            g_entities[i].transform.position.y += g_entities[i].velocity.y;

            if (clk::g_time.frame_timer[g_entities[i].frames[3]] == 0)
            {
                g_entities[i].frames[0]++;
                if ( g_entities[i].frames[0] > (g_entities[i].frames[1] + g_entities[i].frames[2]))
                {
                    g_entities[i].frames[0] = 0;
                }
            }
        }
    }
}