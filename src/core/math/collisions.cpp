#include "core/math/collisions.h"
#include <algorithm>

namespace math
{
    bool collision_circleAABB(adobo::vec2f &circleCenter, float radius, adobo::vec4f AABB)
    {
        // Step 1: Clamp circle center to box
        float px = std::max(AABB.x, std::min(circleCenter.x, AABB.z));
        float py = std::max(AABB.y, std::min(circleCenter.y, AABB.w));
    
        // Step 2: Distance from clamped point to circle center
        float dx = px - circleCenter.x;
        float dy = py - circleCenter.y;
    
        // Step 3: Collision if distance <= radius
        return (dx*dx + dy*dy) <= (radius * radius);
    }
}